#ifndef YOCTO_IPSO_FIELD1D_INCLUDED
#define YOCTO_IPSO_FIELD1D_INCLUDED 1

#include "yocto/ipso/field.hpp"

namespace yocto
{
    namespace ipso
    {

        template <typename T>
        class field1D : public field<T>, public patch1D
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            //! construct a field or link it is usr!=NULL
            inline explicit field1D(const patch1D &p,
                                    void          *usr=NULL) :
            field<T>(),
            patch1D(p),
            item(0),
            wksp(0),
            wlen(0)
            {
                if(!usr)
                {
                    wlen  = this->items * sizeof(type);
                    wksp  = memory::kind<memory::global>::acquire(wlen);
                    this->entry = static_cast<type*>(wksp);
                }
                else
                {
                    this->entry = static_cast<type*>(usr);
                }
                item  = this->entry - p.lower;
                this->set_bytes(this->items);
            }

            inline virtual ~field1D() throw()
            {
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


        private:
            type  *item;
            void  *wksp;
            size_t wlen;

            YOCTO_DISABLE_COPY_AND_ASSIGN(field1D);
        };

    }
}

#endif
