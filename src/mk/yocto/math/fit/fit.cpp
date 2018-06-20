
#include "yocto/math/fit/fit.hpp"



namespace yocto
{
    namespace math
    {
        
        namespace Fit
        {

            Variable:: Variable(const string &n, const string &l, const size_t idx) :
            name(n), link(l), indx(idx)
            {
            }

            Variable:: ~Variable() throw()
            {
            }

            Variable:: Variable(const Variable &other) :
            name(other.name), link(other.link), indx(other.indx)
            {
            }

            const string & Variable:: key() const throw()
            {
                return name;
            }
        }

    }
}

#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace math
    {

        namespace Fit
        {
            Variables:: ~Variables() throw() {}
            Variables::  Variables(const size_t n) : VariablesType(n,as_capacity) {}

            Variables:: Variables(const Variables &other) :
            VariablesType(other)
            {
                
            }
            
            
            Variables & Variables:: operator=( const Variables &other)
            {
                Variables tmp(other);
                swap_with(tmp);
                return *this;
            }
            
            Variables & Variables:: operator()(const string &vname, const string &vlink)
            {

                check_no_multiple_link(vlink);
                const size_t      vindx = 1+size();
                const Variable    fv(vname,vlink,vindx);
                if(!insert(fv))
                {
                    throw exception("%s: multiple variable name '%s'", this->name(),  *vname);
                }
                return *this;
            }

            void Variables:: check_no_multiple_link(const string &link) const
            {
                for(const_iterator i=begin(); i!=end();++i)
                {
                    if(link==(*i).link)
                    {
                        throw exception("%s: multiple variable link '%s'", this->name(), *link);
                    }
                }
            }

            Variables & Variables:: operator()(const string &vname)
            {
                return (*this)(vname,vname);
            }

            Variables & Variables:: operator<<(const string &vname)
            {
                return (*this)(vname,vname);
            }

            void Variables:: LocalToGlobal(Indices         &indices,
                                           const Variables &local,
                                           const Variables &global)
            {
                const size_t num_local  = local.size();
                const size_t num_global = global.size();
                indices.make(num_local);
                size_t indx=1;
                for(Variables::const_iterator i=local.begin();indx<=num_local;++i,++indx)
                {
                    const Variable &lfv = *i;
                    if(indx!=lfv.indx)
                    {
                        throw exception("%s: corrupted local fit variable index!!!", local.name());
                    }

                    const Variable *gfv_p = global.search(lfv.link);
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

            size_t Variables:: operator[](const string &var_name) const
            {
                const Variable *fv = search(var_name);
                if(!fv)
                {
                    throw exception("no Variables['%s']",*var_name);
                }
                return fv->indx;
            }

            size_t Variables:: operator[](const char *var_name) const
            {
                const string vn(var_name);
                return (*this)[vn];
            }

            size_t Variables:: getMaxNameLength() const throw()
            {
                size_t ans = 0;
                for( const_iterator i=begin();i!=end();++i)
                {
                    ans = max_of(ans,(*i).name.size());
                }
                return ans;
            }


        }
    }

}

namespace yocto
{
    namespace math
    {

        namespace Fit
        {

            SampleInfo:: SampleInfo(const size_t n) :
            variables(n),
            indices(n,as_capacity)
            {
            }

            SampleInfo:: ~SampleInfo() throw()
            {
            }

            void SampleInfo:: link_to(const Variables &global)
            {
                Variables::LocalToGlobal(indices,variables,global);
            }


        }

    }

}



