#include "yocto/pack/huffman.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/code/unroll.hpp"

#include <iostream>
#include <iomanip>

#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/graphviz.hpp"

namespace yocto {

    namespace pack
    {

        std::ostream & operator<<(std::ostream &os, const Huffman::Node &node)
        {
            os << Huffman::GetChar(node.symb);
            os << " : ";
            os << std::setw(10) << node.freq;
            os << " #" << node.bits;
            os << "  : ";
            for(size_t i=node.bits;i>0;--i)
            {
                const unsigned bit = 1 << (i-1);
                if(0!=(bit&node.code))
                {
                    os << '1';
                }
                else
                {
                    os << '0';
                }
            }
            return os;
        }

        Huffman:: Alphabet:: ~Alphabet() throw()
        {
        }

        Huffman:: Alphabet:: Alphabet() :
        count(0),
        root(0),
        nodes()
        {
            initialize();
        }

        void Huffman::Alphabet::initialize() throw()
        {
            count = 0;
            root  = 0;
            for(size_t i=0;i<MaxBytes;++i)
            {
                Node &node = nodes[i];
                node.left   = 0;
                node.right  = 0;
                node.parent = 0 ;
                node.code   = i;
                node.freq   = 0;
                node.bits   = 8;
                node.symb   = i;
            }

            for(size_t i=MaxBytes;i<MaxItems;++i)
            {
                Node &node  = nodes[i];
                node.left   = 0;
                node.right  = 0;
                node.parent = 0 ;
                node.code   = i;
                node.freq   = 1;
                node.bits   = 0;
                node.symb   = i;
            }
            

        }


        static inline bool __is_special(const Huffman::CharType C) throw()
        {
            return (C=='\\');
        }

        const char *Huffman:: GetChar(const CharType ch) throw()
        {
            static char tmp[4];
            if( (ch>=32&&ch<127) && !__is_special(ch) )
            {
                tmp[0] = '\'';
                tmp[1] = char(ch);
                tmp[2] = '\'';
                tmp[3] = 0;
                return tmp;
            }
            else
            {
                switch(ch)
                {
                    case NYT: return "NYT";
                    case END: return "END";
                    default:
                        break;
                }
                if(ch<MaxBytes)
                {
                    const char *txt = hexa_text_lower[ch];
                    tmp[0] = 'x';
                    tmp[1] = txt[0];
                    tmp[2] = txt[1];
                    tmp[3] = 0;
                    return tmp;
                }
                else
                {
                    return "INS";
                }
            }
        }

        void Huffman:: Alphabet:: display() const
        {
            std::cerr << "Huffman::Alphabet:" << std::endl;
            for(size_t i=0;i<MaxItems;++i)
            {
                const Node &node = nodes[i];
                if(node.freq>0)
                {
                    //std::cerr << "\t" << GetChar(node.symb) << " : " << node.freq << std::endl;
                    std::cerr << "\t" << node << std::endl;
                }
            }
        }

        void Huffman:: Alphabet:: increase(uint8_t B) throw()
        {
            Node &node = nodes[B];
            if(node.freq>0)
            {
                ++(node.freq);
            }
            else
            {
                assert(count<MaxBytes);
                node.freq = 1;
                if(++count>=MaxBytes)
                {
                    nodes[NYT].freq = 0;
                }
            }
        }


        void Huffman:: Alphabet:: rescale() throw()
        {
#define Y_HUFF_RESCALE(I) Node &node = nodes[I]; if(node.freq>0)  (node.freq>>=1) |= 1
            YOCTO_LOOP_FUNC_(MaxBytes,Y_HUFF_RESCALE,0);
#undef Y_HUFF_RESCALE

        }

        void Huffman:: Alphabet:: build_tree()
        {
            static const CodeType __ONE = 1;
            
            // initialize heap
            nheap.free();
#define Y_HUFF_ENQUEUE(I) Node *node = &nodes[I]; if(node->freq>0) { node->code=0; node->bits=1; node->parent = 0; nheap.push(node); }
            YOCTO_LOOP_FUNC_(MaxItems,Y_HUFF_ENQUEUE,0);

            // build tree
            size_t iNode = MaxItems;
            while(nheap.size()>=2)
            {
                Node *right   = nheap.pop();
                Node *left    = nheap.pop();
                Node *parent  = &nodes[iNode++];
                parent->freq  = right->freq + left->freq;
                parent->left  = left;
                parent->right = right;
                parent->symb  = -1;
                parent->code  =  0;
                const size_t  rbits = right->bits;
                const size_t  lbits = left->bits;
                left->code    = 0;
                right->code   = __ONE << (rbits-1);
                parent->bits  = 1+max_of(rbits,lbits);
                left->parent = right->parent = parent;
                std::cerr << "\tLeft =" << *left  << std::endl;
                std::cerr << "\tRight=" << *right << std::endl;

                nheap.push(parent);
            }
            assert(1==nheap.size());
            root = nheap.pop();
            
            for(size_t i=0;i<MaxBytes;++i)
            {
                Node &node = nodes[i];
                if(node.freq>0)
                {
                    Node *curr = &node;
                    node.bits  = 0;
                    for(curr=curr->parent;curr;curr=curr->parent)
                    {
                        ++node.bits;
                        node.code |= curr->code;
                    }
                }
            }

            
        }

        void Huffman:: Alphabet:: graph_tree() const
        {
            const string outName = "huff.dot";
            {
                ios::wcstream fp(outName);
                fp << "digraph G {\n";
                if(root)
                {
                    for(const Node *node = nodes; node <= root; ++node)
                    {
                        if(node->freq<=0) continue;
                        fp.viz(node); fp("[label=\"%s:%u\"];\n", GetChar(node->symb), unsigned(node->freq));
                    }

                    for(const Node *node = nodes; node <= root; ++node)
                    {
                        if(node->freq<=0) continue;
                        if(node->left)
                        {
                            fp.viz(node); fp << "->"; fp.viz(node->left); fp << "[label=\"0\"]\n";
                        }
                        if(node->right)
                        {
                            fp.viz(node); fp << "->"; fp.viz(node->right); fp << "[label=\"1\"]\n";
                        }
                    }
                }
                fp << "}\n";
            }
            ios::graphviz_render(outName);
        }

    }
}
