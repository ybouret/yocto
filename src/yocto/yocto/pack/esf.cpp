#include "yocto/pack/esf.hpp"
#include "yocto/memory/global.hpp"
#include "yocto/code/utils.hpp"
#include <cstring>
#include <iostream>

namespace yocto
{
    namespace pack
    {

        ESF::Alphabet::Alphabet() :
        itemDataOffset(0),
        itemDataLength(MaxItems*sizeof(Item)),
        itemNodeOffset( memory::align(itemDataOffset + itemDataLength) ),
        itemNodeLength(MaxItems*sizeof(ItemNode)),
        wlen( memory::align(itemNodeOffset+itemNodeLength) ),
        wksp( memory::kind<memory::global>::acquire_as<uint8_t>(wlen)  ),
        items( (Item *) &wksp[itemDataOffset] ),
        count(0),
        iList(),
        iPool(),
        nodes( (ItemNode *) &wksp[itemNodeOffset] )
        {
            reset();
        }

        ESF::Alphabet:: ~Alphabet() throw()
        {
            iPool.reset();
            iList.reset();
            memory::kind<memory::global>::release_as(wksp,wlen);
        }


        void ESF:: Alphabet:: reset() throw()
        {
            memset(wksp,0,wlen);
            iPool.reset();
            iList.reset();
            for(size_t i=0;i<MaxItems;++i)
            {
                items[i].code = i;
                iPool.store( nodes+i );
            }
        }

        void ESF:: Alphabet:: increase(const CharType ch) throw()
        {
            assert(ch>=0);
            assert(ch<MaxItems);
            Item *item = &items[ch];
            if(item->freq>0)
            {
                ++(item->freq);
                update(item->node);
            }
            else
            {
                if(ch<MaxBytes)
                {
                    assert(count<MaxBytes);
                    ++count;
                }
                assert(iPool.size>0);
                ItemNode *node = (item->node = iPool.query());
                node->item     = item;
                item->freq     = 1;
                iList.push_back(node);
            }
        }

        void ESF::Alphabet:: update(ItemNode *node) throw()
        {
            assert(iList.size>0);
            assert(iList.owns(node));
            while(node->prev&&node->prev->item->freq<node->item->freq)
            {
                iList.towards_head(node);
            }

        }

        void ESF::Alphabet:: display_items() const
        {
            std::cerr << "ESF_Alpha:" << std::endl;
            for(size_t i=0;i<MaxBytes;++i)
            {
                assert(items[i].code==i);
                if(items[i].freq>0)
                {
                    std::cerr << '\t';
                    if(i>=32&&i<127)
                    {
                        std::cerr << '\'' << char(i) << '\'';
                    }
                    else
                    {
                        std::cerr << 'x' << hexa_text[i];
                    }
                    std::cerr << " : " << items[i].freq << std::endl;
                }
            }
            if(items[NYT].freq>0)
            {
                std::cerr << "\tNYT : " << items[NYT].freq << std::endl;
            }
            if(items[END].freq>0)
            {
                std::cerr << "\tEND : " << items[END].freq << std::endl;
            }
        }

        void ESF::Alphabet:: display_ilist() const
        {
            std::cerr << "ESF_iList:" << std::endl;
            for(const ItemNode *node = iList.head; node; node=node->next)
            {
                const Item     &item = *(node->item);
                const CharType  ch   = item.code;
                std::cerr << '\t';
                if(ch>=32&&ch<127)
                {
                    std::cerr << '\'' << char(ch) << '\'';
                }
                else
                {
                    switch(ch)
                    {
                        case NYT: std::cerr << "NYT"; break;
                        case END: std::cerr << "END"; break;
                        default:
                            std::cerr << 'x' << hexa_text[ch&0xff];
                    }
                }
                std::cerr << " : " << item.freq << std::endl;
            }
        }

        
    }

}
