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
            // clean up
            memset(wksp,0,wlen);
            iPool.reset();
            iList.reset();
            count = 0;
            for(size_t i=0;i<MaxItems;++i)
            {
                assert(0==items[i].code);
                assert(0==items[i].freq);
                items[i].code = i;
                iPool.store( nodes+i );
            }

            // prepare
        }

        void ESF:: Alphabet:: increase(const CharType ch) throw()
        {
            assert(ch<MaxBytes);
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
                // prepare the node
                ItemNode *node = (item->node = iPool.query());
                node->item     = item;
                item->freq     = 1;

                //prepare insertion
                ItemList iTemp;
                while(iList.tail&&iList.tail->item->code>=MaxBytes)
                {
                    iTemp.push_front( iList.pop_back() );
                }
                iList.push_back(node);

                // restore state
                iList.merge_back(iTemp);
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

        void ESF::Alphabet:: rescale() throw()
        {
            for(ItemNode *node = iList.head;node;node=node->next)
            {
                FreqType &freq = node->item->freq;
                assert(freq>0);
                (freq >>= 1) |= 1;
            }
        }


        const char * ESF:: get_text(const CharType ch) throw()
        {
            static char tmp[4];
            if(ch>=32&&ch<127)
            {
                tmp[0] = tmp[2] = '\'';
                tmp[1] = ch;
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
                    {
                        const char *txt = hexa_text_lower[ch&0xff];
                        tmp[0] = 'x';
                        tmp[1] = txt[0];
                        tmp[2] = txt[1];
                        tmp[3] = 0;
                        return tmp;
                    }
                }
            }
        }

        void ESF::Alphabet:: display_items() const
        {
            std::cerr << "ESF_Alpha:" << std::endl;
            for(size_t i=0;i<MaxItems;++i)
            {
                assert(items[i].code==i);
                if(items[i].freq>0)
                {
                    std::cerr << "\t" << get_text(items[i].code) << " : " << items[i].freq << std::endl;
                }
            }

        }

        void ESF::Alphabet:: display_ilist() const
        {
            std::cerr << "ESF_iList:" << std::endl;
            for(const ItemNode *node = iList.head; node; node=node->next)
            {
                const Item     &item = *(node->item);
                std::cerr << "\t" << get_text(item.code) << " : " << item.freq << std::endl;
            }
        }

        
    }

}
