
#include "yocto/chemical/library.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"
#include <cmath>

namespace yocto
{
    namespace chemical
    {
        library:: ~library() throw() {}

        library:: library( const string &id, const size_t reserved ) :
        species_set(reserved,as_capacity),
        name(id),
        max_name_length(0),
        proton_name("H+")
        {
        }


        species & library:: add( const string &sp_name, const int sp_z )
        {
            compile();
            species::pointer p( new species(sp_name,sp_z) );
            if( ! insert(p) )
            {
                throw exception("library: multiple '%s'", *sp_name);
            }
            (size_t &)(p->indx) = size();
            (size_t &)max_name_length = max_of(max_name_length,p->name.length());
            return *p;
        }

        void library:: compile() throw()
        {
            size_t &m = (size_t&)max_name_length;
            size_t j=0;
            m = 0;
            for(iterator i=begin();i!=end();++i)
            {
                species &s = **i;
                (size_t&)(s.indx) = ++j;
                m = max_of<size_t>(m,s.name.length());
            }
        }

        species & library:: operator()(const string &id)
        {
            species::pointer *p = search(id);
            if(!p)
            {
                throw exception("library: no '%s'", *id);
            }
            return **p;
        }

        species & library:: operator()(const char *id)
        {
            const string ID(id);
            return (*this)(ID);
        }

        std::ostream & operator<<( std::ostream &os, const library &lib)
        {
            os << '<' << lib.name << '>' << std::endl;
            for( library::const_iterator i=lib.begin();i!=lib.end();++i)
            {
                const species &sp = **i;
                os << '\t' << sp.name; for(size_t j=sp.name.length();j<=lib.max_name_length;++j) os << ' ';
                os << ':' << ' '  << sp.z << std::endl;
            }
            os << '<' << lib.name << '/' << '>' << std::endl;
            return os;
        }

        void library:: display( const array<double> &C ) const
        {
            assert(C.size()>=size());
            size_t ii=1;
            for( library::const_iterator i=begin();i!=end();++i,++ii)
            {
                const species &sp = **i;
                assert(sp.indx==ii);
                std::cerr << '[' << sp.name << ']';
                for(size_t j=sp.name.length();j<=max_name_length;++j) std::cerr << ' ';
                std::cerr << '=' << ' ' << C[ii] << std::endl;
            }
        }

        double library:: pH(const array<double> &C) const
        {
            static const char fn[] = "chemical.library.pH: ";
            const species::pointer *ppProton = search(proton_name);
            if(!ppProton) throw exception("%schemical.library.pH: no %s",fn,*proton_name);
            const size_t proton_index = (**ppProton).indx;
            assert(proton_index>0);
            assert(proton_index<=C.size());
            const double h = C[proton_index];
            if(h<=0) throw exception("%s[%s]=%g",fn,*proton_name, h);
            return -log10(h);
        }


    }
}

