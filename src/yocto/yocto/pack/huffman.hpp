#ifndef YOCTO_HUFFMAN_INCLUDED
#define YOCTO_HUFFMAN_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/code/round.hpp"
#include "yocto/ios/bitio.hpp"
#include "yocto/pack/q-codec.hpp"

namespace yocto
{
    namespace pack
    {

        struct Huffman
        {
            typedef size_t        FreqType; //! for frequency counts
            typedef unit_t        CharType; //! [0..255+extra]
            typedef size_t        CodeType; //! encoded char
            static const size_t   NumBytes = 256;
            static const size_t   LastByte = NumBytes-1;
            static const CharType NYT      = LastByte+1;
            static const CharType EOS      = NYT+1;
            static const size_t   NumChars = EOS+1;
            static const size_t   NumNodes = 2*NumChars;

            static const char *Text(const CharType ch) throw(); //!< not thread safe

            //! hold information about a char
            class CharNode
            {
            public:
                CharNode      *next; //!< for list
                CharNode      *prev; //!< for list
                CharType       Char; //!< mapping to a character
                FreqType       Freq; //!< its frequency
                CodeType       Code; //!< its code
                size_t         Bits; //!< ist code size
                inline void initialize(const FreqType f) throw()
                {
                    next = 0;
                    prev = 0;
                    Freq = f;
                }
            private:
                CharNode(); ~CharNode() throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(CharNode);
            };
            typedef core::list_of<CharNode> CharList;

            static const CodeType NodeAtLeft  = 0;
            static const CodeType NodeAtRight = 1;

            //! a Huffman tree node
            class TreeNode
            {
            public:
                TreeNode       *parent;
                TreeNode       *left;
                TreeNode       *right;
                TreeNode       *prev;
                TreeNode       *next;
                const CharNode *Node; //!< not null if leaf, null if internal
                FreqType        Freq;
                size_t          Bits;
                CodeType        cbit; //!< setup during build
            private:
                TreeNode(); ~TreeNode() throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(TreeNode);
            };
            typedef core::list_of<TreeNode> NodeList;

            //! manage operations
            class Alphabet
            {
            public:
                explicit Alphabet();
                virtual ~Alphabet() throw();

                void initialize() throw();           //!< reset frequencies and initialize used
                void display() const;                //!< debug output
                void add( const uint8_t b ) throw(); //!< for debug purpose: append or update

                const CharNode & operator[](const uint8_t b) const throw();
                void append( const CharNode &ch ) throw(); //!< with a zero frequency,     and build tree
                void update( const CharNode &ch ) throw(); //!< with a positive frequency, and build tree

                void encode( ios::bitio &io, const uint8_t b);
                void flush( ios::bitio &io );

                void buildTree() throw();
                void saveTree() const;
                void rescale() throw();

                enum DecodeStatus
                {
                    DecodeWaitFor8,
                    DecodeWaitFor1
                };

                class DecodeContext
                {
                public:
                    inline  DecodeContext() throw() : node(0), status(DecodeWaitFor8) {}
                    inline ~DecodeContext() throw() {}
                    inline  void restart() throw() { node=0; status=DecodeWaitFor8; }
                    TreeNode     *node;
                    DecodeStatus  status;

                private:
                    YOCTO_DISABLE_COPY_AND_ASSIGN(DecodeContext);
                };

                bool decode( ios::bitio &io, DecodeContext &ctx, char &C );

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Alphabet);
                static const size_t   CharNodeLength = NumChars * sizeof(CharNode);
                static const size_t   TreeNodeOffset = YOCTO_MEMALIGN(CharNodeLength);
                static const size_t   RequiredLength = TreeNodeOffset + NumNodes * sizeof(TreeNode);
                size_t    max_bits;
                TreeNode *root;
                CharList  used;    //!< current used chars, ranked by frequencies
                size_t    size;    //!< alphabet size
                CharNode *nyt;
                CharNode *eos;
                size_t    count; //!< for memory allocation
                uint8_t  *bytes; //!< for chars+nodes
                CharNode *chars; //!< NumChars
                TreeNode *nodes; //!< NumNodes

            };

            class Codec : public pack::q_codec
            {
            public:
                virtual ~Codec() throw();
                virtual void reset() throw();
                
            protected:
                explicit Codec();
                Alphabet   ab;
                ios::bitio io;
                void io2Q();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Codec);
            };

            class Encoder : public Codec
            {
            public:
                explicit Encoder();
                virtual ~Encoder() throw();

                virtual void write( char C );
                virtual void flush();
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Encoder);
            };

            class Decoder : public Codec
            {
            public:
                explicit Decoder();
                virtual ~Decoder() throw();

                virtual void write(char C);
                virtual void flush();
                virtual void reset() throw();
                
            private:
                Alphabet::DecodeContext ctx;
                YOCTO_DISABLE_COPY_AND_ASSIGN(Decoder);
            };

        };

    }
}

#endif

