#include "yocto/pack/esf.hpp"
#include "yocto/memory/global.hpp"
#include <cstring>

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
            if(item->freq)
            {
                ++(item->freq);
                // update list
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
            
        }


        
    }

}
