
#include "yocto/math/fit/fit.hpp"
#include "yocto/math/ztype.hpp"

namespace yocto
{
    namespace math
    {
        namespace Fit
        {
            template <> SampleType<real_t>:: ~SampleType() throw() {}
            template <> SampleType<real_t>::  SampleType(const size_t n) :
            SampleInfo(n),
            u(n,as_capacity),
            beta(n,as_capacity),
            curv()
            {
            }

            template <>
            void SampleType<real_t>:: allocate(const size_t nvar)
            {
                u.   make(nvar);
                beta.make(nvar);
                curv.make(nvar); //! large model
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

            template <>
            real_t Gradient<real_t>:: eval_f(const real_t x, const Array &a)
            {
                assert(var);
                assert(pfn);
                return (*pfn)(x,a,*var);
            }

            template <>
            Gradient<real_t>:: Gradient() :
            derivatives<real_t>(),
            scale( REAL(1.0e-4) ),
            pfn(0),
            var(0),
            dh(),
            f( this, & Gradient<real_t>::eval_f )
            {
            }

            template <>
            Gradient<real_t>:: ~Gradient() throw()
            {
            }

            template <>
            void Gradient<real_t>::  compute(Array           &dFda,
                                             Function        &F,
                                             const real_t     x,
                                             const Array     &a,
                                             const Variables &v)
            {
                derivatives<real_t> &drvs = *this;
                pfn = &F;
                var = &v;
                dh.make(a.size(),scale);
                drvs.gradient(dFda,f, x, a, dh);
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

            template <> Sample<real_t>:: ~Sample() throw() {}


            template <> Sample<real_t>:: Sample(const Array &userX,
                                                const Array &userY,
                                                Array       &userZ,
                                                const size_t capa) :
            SampleType<real_t>(capa),
            X(userX), Y(userY), Z(userZ)
            {

            }

            template <>
            void Sample<real_t>:: link()
            {
                link_to(variables);
                allocate(variables.size());
            }

            template <>
            void Sample<real_t>:: assign(const Array &a) const
            {
                assert(u.size()==variables.size()       || die("not linked") );
                assert(indices.size()==variables.size() || die("not linked") );
                for(size_t i=u.size();i>0;--i)
                {
                    const size_t j=indices[i];
                    assert(j>0        ||die("corrupted indices"));
                    assert(j<=a.size()||die("corrupted link")   );
                    u[i] = a[j];
                }
            }


            template <>
            real_t Sample<real_t>::computeD2(Function           &F,
                                             const Array        &a) const
            {
                assign(a);
                const size_t N = X.size();
                assert(Y.size()==N);
                assert(Z.size()==N);
                real_t D2 = 0;
                for(size_t j=N;j>0;--j)
                {
                    const real_t del = Y[j] - (Z[j]=F(X[j],u,variables));
                    D2 += del*del;
                }
                return D2;
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

            template <> Samples<real_t>:: ~Samples() throw() {}
            template <> Samples<real_t>::  Samples(const size_t capa_samples, const size_t capa_variables) :
            SampleType<real_t>(capa_variables),
            Sample<real_t>::Collection(capa_samples,as_capacity)
            {
            }

            template <>
            Sample<real_t> & Samples<real_t>::add(const Array &userX, const Array &userY, Array &userZ, const size_t capa_local)
            {
                Sample<real_t>::Pointer pS( new Sample<real_t>(userX,userY,userZ,capa_local) );
                push_back(pS);
                return *pS;
            }

            template <>
            void Samples<real_t>:: link()
            {
                array< Sample<real_t>::Pointer > &self = *this;
                link_to(variables);
                allocate(variables.size());
                for(size_t i=size();i>0;--i)
                {
                    Sample<real_t> &S = *self[i];
                    S.link_to(variables);
                    S.allocate( S.variables.size() );
                }
            }


            template <>
            real_t Samples<real_t>::computeD2(Function           &F,
                                              const Array        &a) const
            {
                const array< Sample<real_t>::Pointer > &self = *this;
                real_t ans = 0;
                for(size_t i=size();i>0;--i)
                {
                    ans += self[i]->computeD2(F,a);
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
            template <>
            LS<real_t>:: ~LS() throw()
            {
            }



            template <>
            LS<real_t>:: LS() :
            Gradient<real_t>(),
            nvar(0),
            psm(0),
            atry()
            {
            }

            template <>
            bool LS<real_t>:: operator()(Sample<real_t>       &sample,
                                         Function             &F,
                                         Array                &aorg,
                                         const array<bool>    &used,
                                         Array                &aerr,
                                         Callback             *cb)
            {

                //______________________________________________________________
                //
                // Initialize
                //______________________________________________________________
                const size_t nvar = sample.variables.size();
                assert(aorg.size()>=nvar);
                assert(used.size()>=nvar);
                assert(aerr.size()>=nvar);
                
                if(nvar<=0)
                {
                    return true;
                }
                sample.link();
                atry.make(nvar);

                
                return true;
            }


        }
    }
}




