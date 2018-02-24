
#include "yocto/math/fit/fit.hpp"
#include "yocto/math/ztype.hpp"

namespace yocto
{
    namespace math
    {
        template <>
        void FitVariables:: build_matrix<real_t>(matrix<real_t>     &J,
                                                 const FitVariables &local,
                                                 const FitVariables &global)
        {
            assert(local.size() >0);
            assert(global.size()>0);
            const size_t M = local.size();
            const size_t N = global.size();
            if(J.make(M,N)) J.ldz();
            for(size_t r=M;r>0;--r)
            {
                J[r][global[local[r]]] = 1;
            }
        }

        template <>
        void FitVariables:: build_vector(vector<real_t>       &lvec,
                                         const array<real_t>  &gvec,
                                         const FitVariables   &local,
                                         const FitVariables   &global)
        {
            assert(gvec.size()==global.size());
            lvec.make(local.size());
            for(size_t l=local.size();l>0;--l)
            {
                lvec[l] = gvec[ global[local[l]] ];
            }
        }
        

        template <> Fit<real_t>::SampleType:: ~SampleType() throw() {}

        template <> Fit<real_t>::SampleType:: SampleType() :
        local(),
        u()
        {
        }
        
    }
}


namespace yocto
{
    namespace math
    {
        template <> Fit<real_t>::Sample:: ~Sample() throw() {}


        template <> Fit<real_t>::Sample:: Sample(const Array &userX,
                                                 const Array &userY,
                                                 Array       &userZ) :
        SampleType(),
        X(userX), Y(userY), Z(userZ)
        {

        }

        template <>
        real_t Fit<real_t>::Sample::computeD2(Function           &F,
                                              const Array        &params,
                                              const FitVariables &global) const
        {
            FitVariables::build_vector(u,params,local,global);
            const size_t N = X.size();
            assert(Y.size()==N);
            assert(Z.size()==N);
            real_t D2 = 0;
            for(size_t j=N;j>0;--j)
            {
                const real_t del = Y[j] - (Z[j]=F(X[j],u));
                D2 += del*del;
            }
            return D2;
        }

    }
}

namespace yocto
{
    namespace math
    {
        template <> Fit<real_t>::Samples:: ~Samples() throw() {}
        template <> Fit<real_t>::Samples::  Samples(const size_t n) :
        SamplesType(n,as_capacity)
        {
        }

        template <>
        real_t Fit<real_t>::Samples::computeD2(Function           &F,
                                              const Array        &params,
                                              const FitVariables &global) const
        {
            const SamplesType &self = *this;
            real_t ans = 0;
            for(size_t i=size();i>0;--i)
            {
                ans += self[i]->computeD2(F,params,global);
            }
            return ans;
        }

        template <>
        Fit<real_t>::Sample & Fit<real_t>:: Samples:: add(const Array &userX, const Array &userY, Array &userZ)
        {
            SamplePointer pS( new Sample(userX,userY,userZ) );
            push_back(pS);
            return *pS;
        }


    }

}


