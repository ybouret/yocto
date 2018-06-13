
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
            if(ch<0||ch>NumChars)
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
                if(ch>=32&&ch<127)
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
        used(),
        full(0),
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
            full = 0;
            for(size_t i=0;i<NumBytes;++i)
            {
                CharNode &ch = chars[i];
                assert(i==ch.Char);
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
            assert(full<NumBytes);
            ++(p->Freq);
            used.insert_before(nyt,p);
            if(++full>=NumBytes)
            {
                std::cerr << "\t=== FULL ===" << std::endl;
                (void) used.unlink(nyt);
            }
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


    }
}


