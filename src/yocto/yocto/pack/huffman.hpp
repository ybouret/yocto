#ifndef YOCTO_HUFFMAN_INCLUDED
#define YOCTO_HUFFMAN_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/code/round.hpp"
#include "yocto/ios/bitio.hpp"

namespace yocto
{
    namespace pack
    {

        struct Huffman
        {
            typedef size_t        FreqType;
            typedef unit_t        CharType;
            typedef size_t        CodeType;
            static const size_t   NumBytes = 256;
            static const size_t   LastByte = NumBytes-1;
            static const CharType NYT      = LastByte+1;
            static const CharType EOS      = NYT+1;
            static const size_t   NumChars = EOS+1;
            static const size_t   NumNodes = 2*NumChars;

            static const char *Text(const CharType ch) throw(); //!< not thread safe


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

            class TreeNode
            {
            public:
                TreeNode       *parent;
                TreeNode       *left;
                TreeNode       *right;
                TreeNode       *prev;
                TreeNode       *next;
                const CharNode *Node;
                FreqType        Freq;
                size_t          Bits;
                CodeType        cbit;
            private:
                TreeNode(); ~TreeNode() throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(TreeNode);
            };
            typedef core::list_of<TreeNode> NodeList;

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
        };

    }
}

#endif

