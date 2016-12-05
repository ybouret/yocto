#include "yocto/alchemy/library.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace alchemy
    {
        library:: library() throw() : species::database(), max_name_length(0)
        {
        }

        library:: library(const library &other):
        species::database(other),
        max_name_length(other.max_name_length)
        {
        }


        library:: ~library() throw()
        {

        }

        species & library:: add( const string &name, const int z)
        {
            const size_t idx = size() + 1;
            species::pointer sp( new species(name,z,idx) );
            if(!insert(sp))
            {
                throw exception("alchemy.library.add(multiple '%s')", name.c_str());
            }
            max_name_length = max_of(max_name_length,name.size());
            return *sp;
        }

        species & library:: add(const char   *name, const int z)
        {
            const string Name(name);
            return add(Name,z);
        }

        species::pointer & library:: operator[](const string &name)
        {
            species::pointer *pps = search(name);
            if(!pps) throw exception("no alchemy.library['%s']", name.c_str());
            return *pps;
        }

        species::pointer & library:: operator[](const char *name)
        {
            const string Name(name);
            return (*this)[Name];
        }

        const species::pointer & library:: operator[](const string &name) const
        {
            const species::pointer *pps = search(name);
            if(!pps) throw exception("no alchemy.library['%s']", name.c_str());
            return *pps;
        }

        const species::pointer & library:: operator[](const char *name) const
        {
            const string Name(name);
            return (*this)[Name];
        }

        size_t library:: index_of(const string &name) const
        {
            return (*this)[name]->indx;
        }

        size_t library:: index_of(const char *name) const
        {
            return (*this)[name]->indx;
        }


    }
}


namespace yocto
{
    namespace alchemy
    {

        double library:: osmolarity_of(const array<double> &C) const throw()
        {
            assert(C.size()>=size());
            const size_t    n  = size();
            double          zC = 0;
            for(register size_t i=1;i<=n;++i)
            {
                zC += C[i];
            }
            return zC;
        }

        double library:: molar_charge_of(const array<double> &C) const throw()
        {
            assert(C.size()>=size());
            const size_t    n  = size();
            const_iterator  it = begin();
            double          zC = 0;
            for(register size_t i=1;i<=n;++i,++it)
            {
                zC += (*it)->z * C[i];
            }
            return zC;
        }

        double library:: ionic_strength_of(const array<double> &C) const throw()
        {
            assert(C.size()>=size());
            const size_t    n  = size();
            const_iterator  it = begin();
            double          z2C = 0;
            for(register size_t i=1;i<=n;++i,++it)
            {
                z2C += (*it)->zsq * C[i];
            }
            return z2C*0.5;
        }

    }

}



