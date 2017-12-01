#ifndef YOCTO_IPSO_FIELD1D_INCLUDED
#define YOCTO_IPSO_FIELD1D_INCLUDED 1

#include "yocto/ipso/patch.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename T>
        class field1D : public patch1D
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            //! construct a field or link it is usr!=NULL
            inline explicit field1D(const patch1D &p,
                                    void          *usr=NULL) :
            patch1D(p),
            entry(0),
            item(0),
            wksp(0),
            wlen(0)
            {
                if(!usr)
                {
                    wlen  = this->items * sizeof(type);
                    wksp  = memory::kind<memory::global>::acquire(wlen);
                    entry = static_cast<type*>(wksp);
                }
                else
                {
                    entry = static_cast<type*>(usr);
                }
                item  = entry - p.lower;
            }

            inline virtual ~field1D() throw()
            {
                entry = NULL;
                item  = NULL;
                memory::kind<memory::global>::release(wksp,wlen);
            }

            inline type & operator[](const coord1D i) throw()
            {
                assert(this->has(i));
                return item[i];
            }

            inline const_type & operator[](const coord1D i) const throw()
            {
                assert(this->has(i));
                return item[i];
            }

            type *entry; //!< base address for this->items T

        private:
            type  *item;
            void  *wksp;
            size_t wlen;

            YOCTO_DISABLE_COPY_AND_ASSIGN(field1D);
        };

    }
}

#endif

