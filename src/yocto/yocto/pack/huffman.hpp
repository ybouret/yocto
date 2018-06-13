#ifndef YOCTO_HUFFMAN_INCLUDED
#define YOCTO_HUFFMAN_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/code/round.hpp"

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
                CharNode      *next;
                CharNode      *prev;
                CharType       Char;
                FreqType       Freq;
                CodeType       Code;
                size_t         Bits;
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

                void initialize() throw(); //!< reset frequencies and initialize used
                void display() const;
                void add( const uint8_t b ) throw();

                const CharNode & operator[](const uint8_t b) const throw();
                void append( const CharNode &ch ) throw(); //!< with a zero frequency
                void update( const CharNode &ch ) throw(); //!< with a positive frequency

                void buildTree() throw();
                void saveTree() const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Alphabet);
                static const size_t   CharNodeLength = NumChars * sizeof(CharNode);
                static const size_t   TreeNodeOffset = YOCTO_MEMALIGN(CharNodeLength);
                static const size_t   RequiredLength = TreeNodeOffset + NumNodes * sizeof(TreeNode);
                TreeNode *root;
                CharList  used;
                size_t    full;
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

