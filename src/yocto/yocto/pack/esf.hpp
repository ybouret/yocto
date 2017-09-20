#ifndef YOCTO_PACK_ESF_INCLUDED
#define YOCTO_PACK_ESF_INCLUDED 1

#include "yocto/core/list.hpp"

namespace yocto
{
    namespace pack
    {

        //! Enhanced Shannon-Fano
        struct ESF
        {
            static const size_t MaxBytes = 256;
            static const size_t NumCtrls = 2;
            static const size_t MaxItems = MaxBytes + NumCtrls;
            typedef int         CharType;
            typedef size_t      FreqType;

            struct Item
            {
                CharType code;
                FreqType freq;
            };

            struct ItemNode
            {
                
            };

            class Alphabet
            {
            public:

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Alphabet);
            };

        };

    }
}

#endif

