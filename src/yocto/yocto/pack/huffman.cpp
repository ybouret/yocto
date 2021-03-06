
#include "yocto/pack/huffman.hpp"
#include "yocto/code/utils.hpp"

#include <cstring>

namespace yocto
{
    namespace pack
    {
        const char * Huffman:: Text(const CharType ch) throw()
        {
            static char data[4];
            memset(data,0,sizeof(data));
            if(ch<0||ch>CharType(NumChars))
            {
                return "???";
            }
            else
            {
                switch(ch)
                {
                    case EOS: return "EOS";
                    case NYT: return "NYT";
                    default:
                        break;
                }
                if( (ch>=32&&ch<127) && ch!='\"')
                {
                    data[0] = '\'';
                    data[1] = char(ch);
                    data[2] = '\'';
                }
                else
                {
                    data[0] = 'x';
                    const char *src = hexa_text_lower[ unsigned(ch) & 0xff ];
                    data[1] = src[0];
                    data[2] = src[1];
                }
                return data;
            }

        }
    }
}

#include "yocto/memory/global.hpp"
#include <iostream>

namespace yocto
{
    namespace pack
    {
        Huffman::Alphabet:: Alphabet() :
        max_bits(16),
        root(0),
        used(),
        size(0),
        nyt(0),
        eos(0),
        count( RequiredLength ),
        bytes( (uint8_t*)memory::kind<memory::global>::acquire(count) ),
        chars( (CharNode *)(&bytes[0]) ),
        nodes( (TreeNode *)(&bytes[TreeNodeOffset] ) )
        {
            for(size_t i=0;i<NumChars;++i)
            {
                chars[i].Char = i;
            }
            nyt = &chars[NYT];
            eos = &chars[EOS];
            initialize();
            buildTree();
        }

        Huffman:: Alphabet:: ~Alphabet() throw()
        {
            used.reset();
            memory::kind<memory::global>::release_as(bytes,count);
            chars=0;
            nodes=0;
        }

        void Huffman::Alphabet::initialize() throw()
        {
            used.reset();
            size = 0;
            for(size_t i=0;i<NumBytes;++i)
            {
                CharNode &ch = chars[i];
                assert(i==size_t(ch.Char));
                ch.initialize(0);
                ch.Code = i;
                ch.Bits = 8;
            }
            nyt->initialize(1);
            eos->initialize(1);
            used.push_front( eos );
            used.push_front( nyt );
        }

        const Huffman::CharNode & Huffman::Alphabet:: operator[](const uint8_t b) const throw()
        {
            return chars[b];
        }

        void Huffman:: Alphabet:: append(const CharNode &ch) throw()
        {
            CharNode *p = (CharNode *)&ch;
            assert(0==p->Freq);
            assert(used.owns(nyt));
            assert(size<NumBytes);
            ++(p->Freq);
            used.insert_before(nyt,p);
            if(++size>=NumBytes)
            {
                //std::cerr << "\t=== FULL ===" << std::endl;
                (void) used.unlink(nyt);
                nyt->Code=0;
                nyt->Bits=0;
                nyt->Freq=0;
            }
            buildTree();
        }

        void Huffman:: Alphabet:: update(const CharNode &ch) throw()
        {
            CharNode *p = (CharNode *)&ch;
            assert(0<p->Freq);
            ++(p->Freq);
            assert(used.owns(p));
            while( (used.head!=p) && (p->Freq>p->prev->Freq) )
            {
                used.towards_head(p);
            }
            buildTree();
        }

        void Huffman:: Alphabet:: add( const uint8_t b ) throw()
        {
            const CharNode &ch = chars[b];
            if(ch.Freq<=0)
            {
                append(ch);
            }
            else
            {
                update(ch);
            }
        }


        void Huffman:: Alphabet:: display() const
        {
            std::cerr << "#used=" << used.size << std::endl;
            for(const CharNode *ch = used.head; ch; ch=ch->next )
            {
                std::cerr << "\t#" << Text(ch->Char) << " = " << ch->Freq << std::endl;
            }
        }

        static inline
        Huffman::TreeNode * popTreeNode(Huffman::NodeList &Q1,
                                        Huffman::NodeList &Q2) throw()
        {
            assert(Q1.size>0||Q2.size>0);
            if(0==Q2.size)
            {
                //______________________________________________________________
                //
                // no nodes in Q2
                //______________________________________________________________
                assert(Q1.size>0);
                return Q1.pop_back();
            }
            else
            {
                assert(Q2.size>0);
                if(0==Q1.size)
                {
                    //__________________________________________________________
                    //
                    // no nodes in Q1
                    //__________________________________________________________
                    return Q2.pop_back();
                }
                else
                {
                    //__________________________________________________________
                    //
                    // nodes in both queue
                    //__________________________________________________________
                    return (Q1.tail->Freq<=Q2.tail->Freq) ? Q1.pop_back() : Q2.pop_back();
                }
            }
        }


        void Huffman:: Alphabet:: rescale_frequencies() throw()
        {
            for(CharNode *ch = used.head; ch; ch=ch->next)
            {
                assert(ch->Freq>0);
                (ch->Freq>>=1)|=1;
            }
        }

        void Huffman:: Alphabet:: buildTree() throw()
        {
            //__________________________________________________________________
            //
            // build the tree
            //__________________________________________________________________
        TRY_BUILD:
            {
                //______________________________________________________________
                //
                // initialize queues
                //______________________________________________________________
                NodeList Q1;
                NodeList Q2;
                size_t   iNode = 0;
                for(const CharNode *ch = used.head; ch; ch=ch->next)
                {
                    assert(iNode<NumNodes);
                    TreeNode *node = &nodes[iNode++];
                    node->parent   = 0;
                    node->left     = 0;
                    node->right    = 0;
                    node->next     = 0;
                    node->prev     = 0;
                    node->Node     = ch;
                    node->Freq     = ch->Freq;
                    node->Bits     = 0;
                    Q1.push_back(node);
                }

                //______________________________________________________________
                //
                // algorithm
                //______________________________________________________________
                while( (Q1.size+Q2.size)>1 )
                {
                    assert(iNode<NumNodes);
                    TreeNode *node = &nodes[iNode++];
                    node->Node     = 0;
                    TreeNode *r    = (node->right = popTreeNode(Q1,Q2));
                    TreeNode *l    = (node->left  = popTreeNode(Q1,Q2));
                    node->Bits = max_of(l->Bits,r->Bits)+1;
                    if(node->Bits>max_bits)
                    {
                        rescale_frequencies();
                        Q1.reset();
                        Q2.reset();
                        goto TRY_BUILD;
                    }

                    node->Freq = l->Freq+r->Freq;
                    r->parent  = l->parent = node;
                    l->cbit    = NodeAtLeft;
                    r->cbit    = NodeAtRight;
                    node->next = 0;
                    node->prev = 0;
                    node->parent = 0;
                    Q2.push_front(node);
                }
                assert(1==Q1.size+Q2.size);
                root = popTreeNode(Q1,Q2);
                assert(used.size*2-1==iNode);
                root->cbit   = 0;
            }

            //__________________________________________________________________
            //
            // build the codes
            //__________________________________________________________________
            {
                size_t iNode = 0;
                for(CharNode *ch = used.head; ch; ch=ch->next,++iNode)
                {
                    ch->Code = 0;
                    ch->Bits = 0;
                    assert(nodes[iNode].Node == ch);
                    for(const TreeNode *curr=nodes+iNode;curr!=root;curr=curr->parent)
                    {
                        assert( ((curr->cbit) | 0x1 )<=0x1);
                        (ch->Code <<= 1) |= curr->cbit;
                        ch->Bits++;
                    }
                }
            }

        }



    }
}

#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/graphviz.hpp"

namespace yocto
{
    namespace pack
    {
        void Huffman::Alphabet:: saveTree() const
        {
            {
                ios::wcstream fp("huff.dot");
                fp << "digraph G {\n";
                const size_t nu = used.size;
                const size_t nn = 2*nu-1;
                for(size_t i=0;i<nn;++i)
                {
                    const TreeNode *node = nodes+i;
                    fp << '\t'; fp.viz(node);
                    fp << '[';
                    if(node->Node)
                    {
                        fp("label=\"%s#%u@%u\"", Text(node->Node->Char), unsigned(node->Freq), unsigned(node->Node->Bits) );
                    }
                    else
                    {
                        fp("label=\"#%u\"", unsigned(node->Freq) );
                    }
                    fp << ']' << ';' << '\n';
                }
                for(size_t i=0;i<nn;++i)
                {
                    const TreeNode *node = nodes+i;
                    if(node->left)
                    {
                        fp << '\t'; fp.viz(node); fp << "->"; fp.viz(node->left); fp("[label=\"%u\"];\n",unsigned(node->left->cbit));
                    }
                    if(node->right)
                    {
                        fp << '\t'; fp.viz(node); fp << "->"; fp.viz(node->right); fp("[label=\"%u\"];\n",unsigned(node->right->cbit));
                    }
                }
                fp << "}\n";
            }
            ios::graphviz_render("huff.dot");
        }
    }
}

namespace yocto
{
    namespace pack
    {
        void Huffman:: Alphabet:: encode(ios::bitio &io, const uint8_t b)
        {
            const CharNode &ch = chars[b];
            if(ch.Freq>0)
            {
                io.push(ch.Code,ch.Bits);
                update(ch);
            }
            else
            {
                if(size>0)
                {
                    io.push(nyt->Code,nyt->Bits);
                }
                io.push(ch.Code,ch.Bits);
                append(ch);
            }
        }

        void Huffman:: Alphabet:: flush( ios::bitio &io )
        {
            if(size>0)
            {
                io.push( eos->Code, eos->Bits );
                io.fill_to_byte_with(false);
            }
        }
    }
}

#include "yocto/exception.hpp"

namespace yocto
{
    namespace pack
    {
        bool Huffman:: Alphabet:: decode( ios::bitio &io, DecodeContext &ctx, char &C )
        {
            static const char fn[] = "Huffman::Alphabet.decode: ";

            switch(ctx.status)
            {
                case DecodeWaitFor8:
                DECODE8:
                {
                    if(io.size()<8) return false;
                    const uint8_t B = io.pop_full<uint8_t>();
                    C = char(B);
                    CharNode &ch = chars[B];
                    if(ch.Freq>0) throw exception("%salready transmitted char %s",fn,Text(ch.Char));
                    append(ch);
                    ctx.status= DecodeWaitFor1;
                    ctx.node  = root;
                    return true;
                }

                case DecodeWaitFor1:
                DECODE1:
                {
                    assert(ctx.node);
                    if(io.size()<=0) return false; // not enough bits
                    ctx.node = (io.pop()? ctx.node->right : ctx.node->left);
                    const CharNode *pch = ctx.node->Node;
                    if(!pch) goto DECODE1;

                    if(pch==nyt)
                    {
                        if(size<=0) throw exception("%sunexpected NYT",fn);
                        ctx.node   = 0;
                        ctx.status = DecodeWaitFor8;
                        goto DECODE8;
                    }
                    else if(pch==eos)
                    {
                        while( 0 != (io.size() & 7) )
                        {
                            io.skip();
                        }
                        assert(DecodeWaitFor1==ctx.status);
                        ctx.node=root;
                        goto DECODE1;
                    }
                    else
                    {
                        assert(pch->Char>=0); assert(pch->Char<256);
                        const uint8_t B = uint8_t(pch->Char);
                        C = char(B);
                        CharNode &ch = chars[B];
                        if(ch.Freq<=0) throw exception("%sunexpected encoded %s",fn,Text(pch->Char));
                        update(ch);
                        assert(DecodeWaitFor1==ctx.status);
                        ctx.node = root;
                        return true;
                    }
                }
            }
            return false;
        }

    }
}


namespace yocto
{
    namespace pack
    {
        Huffman:: Codec:: Codec() : ab(), io()
        {
        }

        Huffman:: Codec:: ~Codec() throw()
        {
        }

        void Huffman:: Codec:: reset() throw()
        {
            io.free();
            ab.initialize();
        }

        void Huffman:: Codec:: io2Q()
        {
            while(io.size()>=8)
            {
                Q.push_back( io.pop_full<uint8_t>() );
            }
        }
    }
}

namespace yocto
{
    namespace pack
    {
        Huffman::Encoder::  Encoder() : Codec() {}
        Huffman::Encoder:: ~Encoder() throw() {}


        void Huffman:: Encoder:: write( char C )
        {
            ab.encode(io,C);
            io2Q();
        }

        void Huffman:: Encoder:: flush()
        {
            ab.flush(io);
            io2Q();
        }

    }

}

namespace yocto
{
    namespace pack
    {
        Huffman::Decoder::  Decoder() : Codec(), ctx() {}
        Huffman::Decoder:: ~Decoder() throw() {}


        void Huffman:: Decoder:: write( char C )
        {
            io.push_full<uint8_t>(C);
            while( ab.decode(io,ctx,C) )
            {
                Q.push_back(C);
            }
        }

        void Huffman:: Decoder:: flush()
        {
            // nothing to do
        }

        void Huffman:: Decoder:: reset() throw()
        {
            Codec::reset();
            ctx.restart();
        }

    }

}


