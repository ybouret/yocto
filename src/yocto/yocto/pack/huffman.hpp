#ifndef YOCTO_PACK_HUFFMAN_INCLUDED
#define YOCTO_PACK_HUFFMAN_INCLUDED 1

#include "yocto/ordered/heap.hpp"

namespace yocto
{
    namespace pack
    {

        struct Huffman
        {
            typedef size_t   FreqType;
            typedef unsigned CharType;
            typedef unsigned CodeType;

            static const size_t   MaxBytes = 256;
            static const size_t   Controls = 2;
            static const size_t   MaxItems = MaxBytes + Controls;
            static const size_t   MaxNodes = 3*MaxItems;
            static const CharType NYT      = MaxBytes+0;
            static const CharType END      = MaxBytes+1;

            static const char *GetChar(const CharType ch) throw();

            struct Node
            {
                Node    *left;
                Node    *right;
                FreqType freq;
                CodeType code;
                size_t   bits;
                CharType symb;
                
                class Comparator
                {
                public:
                    inline  Comparator() throw() {}
                    inline ~Comparator() throw() {}
                    inline int operator()(const Node &lhs, const Node &rhs) throw()
                    {
                        return unit_t(rhs.freq)-unit_t(lhs.freq);
                    }
                private:
                    YOCTO_DISABLE_COPY_AND_ASSIGN(Comparator);
                };
                typedef heap<Node,Node::Comparator> Heap;
            };

            class Alphabet
            {
            public:
                Alphabet();
                ~Alphabet() throw();
                void initialize() throw();

                void display() const;
                void increase(uint8_t B) throw();
                void rescale() throw();
                void build_tree();
                
            private:
                size_t     count; //!< used alphabet size
                Node::Heap nheap;
                Node       nodes[MaxNodes];

            };

        };

    }
}

#endif

