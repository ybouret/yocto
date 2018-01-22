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

#define YOCTO_IPSO_FIELD1D_CTOR(ID) \
field<T>(ID),                        \
patch1D(p),                           \
item(0),                               \
wksp(0),                                \
wlen(0)

            //! construct a field or link it is usr!=NULL
            inline explicit field1D(const char    *id,
                                    const patch1D &p,
                                    void          *usr=NULL) :
            YOCTO_IPSO_FIELD1D_CTOR(id)
            {
                build_with(p,usr);
            }

            inline explicit field1D(const string &id,
                                    const patch1D &p,
                                    void          *usr = NULL ) :
            YOCTO_IPSO_FIELD1D_CTOR(*id)
            {
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

            template <typename U>
            inline void load( const field1D<U> &F, const patch1D &zone) throw()
            {
                assert(this->contains(zone));
                T *       self = this->item;
                const U * peer = F.item;
                for(unit_t x=zone.upper;x>=zone.lower;--x)
                {
                    self[x] = static_cast<T>(peer[x]);
                }
            }

            template <typename U>
            inline void save( field1D<U> &F, const patch1D &zone) const throw()
            {
                assert(this->contains(zone));
                const T *self = this->item;
                U       *peer = F.item;
                for(unit_t x=zone.upper;x>=zone.lower;--x)
                {
                    peer[x] = static_cast<U>(self[x]);
                }
            }


        private:
            type  *item;
            void  *wksp;
            size_t wlen;

            YOCTO_DISABLE_COPY_AND_ASSIGN(field1D);
            inline void build_with(const patch1D &p,void *usr)
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
        };

    }
}

#endif

