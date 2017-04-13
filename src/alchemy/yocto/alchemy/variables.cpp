#include "yocto/alchemy/variables.hpp"

namespace yocto
{
    namespace alchemy
    {
        variable:: ~variable() throw() {}

        variable::variable(const size_t iv, const char *id, const char *ld) :
        indx(iv),
        name(id),
        load(ld)
        {
        }

        variable::variable(const size_t iv, const string &id, const string &ld) :
        indx(iv),
        name(id),
        load(ld)
        {
        }

        const string & variable:: key() const throw()
        {
            return name;
        }
        
    }
    
}

#include "yocto/exception.hpp"

namespace yocto
{
    namespace alchemy
    {
        variables:: ~variables() throw()
        {
        }

        size_t variables:: operator[](const string &name) const
        {
            const variable::ptr *ppVar = db.search(name);
            if(!ppVar)
            {
                throw exception("no variable '%s'", name.c_str());
            }
            return (*ppVar)->indx;
        }

        size_t variables:: operator[](const char *name) const
        {
            const string Name(name);
            return (*this)[Name];
        }


        variables::iterator variables::begin() const throw() { return db.begin(); }
        variables::iterator variables::end()   const throw() { return db.end();   }


#define YCHEM_VARIABLES_CTOR(N) \
shlib(a_lib), count(shlib->size()),extra(0),db(N,as_capacity)

        variables:: variables(const library::pointer &a_lib,
                              const char             *names[],
                              const char             *loads[],
                              const size_t            num) :
        YCHEM_VARIABLES_CTOR(num)
        {
            for(size_t i=0;i<num;++i)
            {
                assert(names);
                assert(loads);
                const string name = names[i];
                const string load = loads[i];
                create_variable(name,load);
            }
        }

        variables:: variables(const library::pointer &a_lib,
                              const array<string>    &names,
                              const array<string>    &loads) :
        YCHEM_VARIABLES_CTOR(names.size())
        {
            assert(names.size()==loads.size());
            for(size_t i=1;i<=names.size();++i)
            {
                create_variable(names[i],loads[i]);
            }
        }


        void variables:: create_variable(const string &name, const string &load)
        {
            const variable::ptr pV( new variable(shlib->size()+db.size()+1,name,load) );
            if(!db.insert(pV))
            {
                throw exception("mutiple variable '%s'", name.c_str());
            }
            ++(size_t&)count;
            ++(size_t&)extra;
        }


        std::ostream & operator<<( std::ostream &os, const variables &params)
        {
            os << "<" << std::endl;
            for(variables::iterator i=params.begin();i!=params.end();++i)
            {
                const variable &v = **i;
                os << " " << v.name << " @ " << v.indx << std::endl;
            }
            os << ">";
            return os;
        }

    }
}
