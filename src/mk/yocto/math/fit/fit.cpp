
#include "yocto/math/fit/fit.hpp"



namespace yocto
{
    namespace math
    {
        

        
        FitVariable:: FitVariable(const string &n, const string &l, const size_t idx) :
        name(n), link(l), indx(idx)
        {
        }

        FitVariable:: ~FitVariable() throw()
        {
        }

        FitVariable:: FitVariable(const FitVariable &other) :
        name(other.name), link(other.link), indx(other.indx)
        {
        }

        const string & FitVariable:: key() const throw()
        {
            return name;
        }

    }
}

namespace yocto
{
    namespace math
    {

        FitVariables:: ~FitVariables() throw() {}
        FitVariables::  FitVariables(const size_t n) : FitVariablesType(n,as_capacity) {}


        FitVariables & FitVariables:: operator()(const string &vname, const string &vlink)
        {

            check_no_multiple_link(vlink);
            const size_t      vindx = 1+size();
            const FitVariable fv(vname,vlink,vindx);
            if(!insert(fv))
            {
                throw exception("%s: multiple variable name '%s'", this->name(),  *vname);
            }
            return *this;
        }

        void FitVariables:: check_no_multiple_link(const string &link) const
        {
            for(const_iterator i=begin(); i!=end();++i)
            {
                if(link==(*i).link)
                {
                    throw exception("%s: multiple variable link '%s'", this->name(), *link);
                }
            }
        }

        FitVariables & FitVariables:: operator()(const string &vname)
        {
            return (*this)(vname,vname);
        }

        FitVariables & FitVariables:: operator<<(const string &vname)
        {
            return (*this)(vname,vname);
        }

        void FitVariables:: LocalToGlobal(FitIndices         &indices,
                                          const FitVariables &local,
                                          const FitVariables &global)
        {
            const size_t num_local  = local.size();
            const size_t num_global = global.size();
            indices.make(num_local);
            size_t indx=1;
            for(FitVariables::const_iterator i=local.begin();indx<=num_local;++i,++indx)
            {
                const FitVariable &lfv = *i;
                if(indx!=lfv.indx)
                {
                    throw exception("%s: corrupted local fit variable index!!!", local.name());
                }

                const FitVariable *gfv_p = global.search(lfv.link);
                if(!gfv_p)
                {
                    throw exception("%s: no link to global variable '%s'", global.name(), *lfv.link);
                }
                const size_t jndx = gfv_p->indx;
                if(jndx<=0||jndx>num_global)
                {
                    throw exception("%s: corrupted global fit variable index!!!", global.name());
                }
                indices[indx] = jndx;
            }
        }
    }

}


#if 0
namespace yocto
{
    namespace math
    {
        
        FitVariables:: ~FitVariables() throw() {}
        FitVariables::  FitVariables(const size_t n) : FitVariablesType(n,as_capacity) {}
        FitVariables::  FitVariables(const FitVariables &other) : FitVariablesType(other) {}

        FitVariables & FitVariables:: operator<<(const string varname)
        {
            if(!insert(varname)) throw exception("FitVariables: no '%s'", *varname);
            return *this;
        }

        FitVariables & FitVariables:: operator<<(const char *varname)
        {
            const string id(varname);
            return ( (*this) << id );
        }

        size_t FitVariables:: find( const string &key, const char *ctx) const
        {
            assert(ctx);
            const size_t ans = index_of(key);
            if(ans<=0) throw exception("FitVariable, in %s: no '%s'", ctx, *key);
            return ans;
        }

        size_t FitVariables:: operator[](const string &key) const
        {
            const size_t ans = index_of(key);
            if(ans<=0) throw exception("not fit variable '%s'", *key);
            return ans;
        }

        size_t FitVariables:: operator[](const char *id) const
        {
            const string key(id);
            return (*this)[key];
        }

    }
}
#endif

