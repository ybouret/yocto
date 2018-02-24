
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

    }
}
