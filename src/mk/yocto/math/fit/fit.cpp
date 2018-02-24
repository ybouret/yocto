
#include "yocto/math/fit/fit.hpp"

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

#if 0
        size_t FitVariables:: find( const string &key, const char *ctx) const
        {
            assert(ctx);
            const size_t ans = index_of(key);
            if(ans<=0) throw exception("FitVariable, in %s: no '%s'", ctx, *key);
            return ans;
        }
#endif
        
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
