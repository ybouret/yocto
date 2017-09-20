#ifndef YOCTO_PACK_ESF_INCLUDED
#define YOCTO_PACK_ESF_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/core/pool.hpp"

namespace yocto
{
    namespace pack
    {

        //! Enhanced Shannon-Fano
        struct ESF
        {
            static const size_t   MaxBytes = 256;
            static const size_t   NumCtrls = 2;
            static const size_t   MaxItems = MaxBytes + NumCtrls;
            typedef unsigned      CharType;
            typedef size_t        FreqType;
            static const CharType END   = MaxBytes+0;
            static const CharType NYT   = MaxBytes+1;

            static const char * get_text(const CharType ch) throw();

            struct ItemNode;

            struct Item
            {
                CharType  code;
                FreqType  freq;
                ItemNode *node;
            };

            struct ItemNode
            {
                ItemNode *next;
                ItemNode *prev;
                Item     *item;
            };
            typedef core::pool_of<ItemNode> ItemPool;
            typedef core::list_of<ItemNode> ItemList;

            class Alphabet
            {
            public:
                explicit Alphabet();
                virtual ~Alphabet() throw();

                void reset() throw();

                void increase(const CharType ch) throw();

                void display_items() const;
                void display_ilist() const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Alphabet);
                const size_t itemDataOffset;
                const size_t itemDataLength;
                const size_t itemNodeOffset;
                const size_t itemNodeLength;
                size_t       wlen;  //!< memory size
                uint8_t     *wksp;  //!< memory workspace
                Item        *items; //!< start of items
                size_t       count; //!< #chars in alphabet
                ItemList     iList; //!< list of ordered char
                ItemPool     iPool; //!< pool of nodes
                ItemNode    *nodes; //!< memory for nodes
                void update(ItemNode *node) throw();
            };

        };

    }
}

#endif

