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

        void Huffman::Node::__viz( ios::ostream &fp ) const
        {
            fp.viz(this);fp("[label=\"%s#%u\"];\n", GetChar(symb), unsigned(freq));
            if(left)
            {
                left->__viz(fp);
                fp.viz(this); fp << "->"; fp.viz(left); fp << "[label=\"0\"]\n";
            }
            if(right)
            {
                right->__viz(fp);
                fp.viz(this); fp << "->"; fp.viz(right); fp << "[label=\"1\"]\n";
            }
        }



        std::ostream & operator<<(std::ostream &os, const Huffman::Node &node)
        {
            os << Huffman::GetChar(node.symb);
            os << " : ";
            os << std::setw(10) << node.freq;
            os << " #" << std::setw(3) << node.bits;
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
            memory::kind<memory::global>::release_as<Node>(nodes,nsize);
        }

        Huffman:: Alphabet:: Alphabet() :
        count(0),
        root(0),
        nheap(),
        nsize(MaxNodes),
        nodes(memory::kind<memory::global>::acquire_as<Node>(nsize)),
        nyt( nodes[NYT] )
        {
            initialize();
        }

        size_t Huffman::Alphabet:: treeBytes() const throw()
        {
            return nsize*sizeof(Node);
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
            return (C=='\\') || (C=='\'') || (C=='\"');
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
                    std::cerr << "\t" << node << std::endl;
                }
            }
        }

        void Huffman:: Alphabet:: increase(const uint8_t B) throw()
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
                    nyt.freq = 0;
                }
            }
        }


        void Huffman:: Alphabet:: rescale() throw()
        {
#define Y_HUFF_RESCALE(I) Node &node = nodes[I]; if(node.freq>0)  (node.freq>>=1) |= 1
            YOCTO_LOOP_FUNC_(MaxBytes,Y_HUFF_RESCALE,0);
#undef Y_HUFF_RESCALE
            assert(nodes[END].freq==1);
        }

        void Huffman:: Alphabet:: build_tree()
        {
        BUILD_TREE:
            {
                //______________________________________________________________
                //
                // initialize heap
                //______________________________________________________________
                nheap.free();


                for(size_t i=0;i<MaxItems;++i)
                {
                    Node *node = &nodes[i];
                    if(node->freq>0)
                    {
                        assert(i==node->symb);
                        node->code   = 0;
                        node->bits   = 0;
                        node->parent = 0;
                        nheap.push(node);
                    }
                }

                //______________________________________________________________
                //
                // build tree
                //______________________________________________________________
                size_t iNode = MaxItems;
                while(nheap.size()>=2)
                {
                    // get children
                    Node *right       = nheap.pop(); assert(0==right->code);
                    Node *left        = nheap.pop(); assert(0==left->code);
                    const size_t bits = 1+max_of(right->bits,left->bits);
                    if(bits>=MaxBits)
                    {
                        rescale();
                        goto BUILD_TREE;
                    }

                    // make parents
                    Node *parent  = &nodes[iNode++];
                    parent->bits  = bits;
                    parent->freq  = (right->freq + left->freq);
                    parent->left  = left;
                    parent->right = right;
                    parent->symb  = -1;
                    parent->code  =  0;
                    right->code   =  1;
                    left->parent  = parent;
                    right->parent = parent;
                    nheap.push(parent);
                }
                assert(1==nheap.size());
                root = nheap.pop();

                //______________________________________________________________
                //
                // deduce codes
                //______________________________________________________________
                for(size_t i=0;i<MaxItems;++i)
                {
                    Node &node = nodes[i];
                    if(node.freq>0)
                    {
                        Node *curr = &node;
                        node.bits  = 0;
                        for(curr=curr->parent;curr;curr=curr->parent)
                        {
                            node.code |= (curr->code << (++node.bits) );
                        }
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
                    root->__viz(fp);
                }
                fp << "}\n";
            }
            ios::graphviz_render(outName);
        }

    }
}

namespace yocto
{
    namespace pack
    {
        Huffman:: AlphaEncoder:: AlphaEncoder() : Alphabet()
        {
        }

        Huffman:: AlphaEncoder:: ~AlphaEncoder() throw()
        {
        }

        void Huffman::AlphaEncoder::encode(const uint8_t B,
                                           ios::bitio   &bio)
        {
            Node &node = nodes[B];
            if(node.freq>0)
            {
                bio.push<CodeType>(node.code,node.bits);
                ++node.freq;
            }
            else
            {
                assert(count<MaxBytes);
                node.freq = 1;
                assert(B==node.code);
                assert(8==node.bits);
                if(count>0&&nyt.bits>0)
                {
                    bio.push<CodeType>(nyt.code,nyt.bits);
                }
                bio.push<CodeType>(node.code,node.bits);
                if(++count>=MaxBytes)
                {
                    nyt.freq=0;
                }
            }
            build_tree();
        }

    }
}
