#include "yocto/hashing/factory.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace hashing
    {
        factory:: ~factory() throw() {}
        factory::  factory() throw() : hfdb(), max_name_length(0) {}
        factory::  factory(const size_t n) : hfdb(n,as_capacity), max_name_length(0) {}

        void factory:: append( function *f )
        {
            const function::pointer p(f);
            const string            k(p->name());
            if(!insert(k,p))
            {
                throw exception("hashing::factory: multiple '%s'",*k);
            }
        }

        function & factory:: operator[](const string &name) const
        {
            const function::pointer *ppf = search(name);
            if(!ppf)
            {
                throw exception("hashing::factory[no '%s']", *name);
            }
            const function &f = **ppf;
            if(name.size()>max_name_length)
            {
                (size_t &)max_name_length = name.size();
            }
            return (function &)f;
        }

        function & factory:: operator[](const char *name) const
        {
            const string id(name);
            return (*this)[id];
        }
    }
}
