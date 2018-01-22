#include "yocto/ipso/field.hpp"

namespace yocto
{
    namespace ipso
    {
        field_info:: ~field_info() throw()
        {
        }
        
        field_info:: field_info(const char *id) :
        name(id),
        count(0),
        bytes(0)
        {
        }

        const string & field_info:: key() const throw()
        {
            return name;
        }

        field_db::  field_db() throw() : field_info::set_type() {}
        field_db:: ~field_db() throw() {}

        field_db:: field_db(const size_t n) : field_info::set_type(n,as_capacity) {}

        field_info       & field_db::  operator[](const string &id)
        {
            field_info::pointer *ppF = search(id);
            if(!ppF) throw exception("field_db: no '%s'", *id);
            return **ppF;
        }

        const field_info & field_db::  operator[](const string &id) const
        {
            const field_info::pointer *ppF = search(id);
            if(!ppF) throw exception("field_db: no CONST '%s'", *id);
            return **ppF;
        }


        field_info       & field_db::  operator[](const char *id)
        {
            const string ID(id); return (*this)[ID];
        }

        const field_info & field_db::  operator[](const char *id) const
        {
            const string ID(id); return (*this)[ID];
        }

    }
}
