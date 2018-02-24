#ifndef YOCTO_MATH_FIT_INCLUDED
#define YOCTO_MATH_FIT_INCLUDED 1

#include "yocto/math/fcn/derivatives.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/string.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace math
    {

        typedef sorted_vector<string> FitVariablesType;
        class FitVariables : public FitVariablesType
        {
        public:
            explicit FitVariables(const size_t n=0);
            virtual ~FitVariables() throw();
            FitVariables & operator<<( const string varname );
            FitVariables & operator<<( const char  *varname );
            FitVariables(const FitVariables &other);

        private:
            YOCTO_DISABLE_ASSIGN(FitVariables);
        };

        template <typename T>
        class Fit
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef array<T>                                 Array;
            typedef vector<T>                                Vector;
            typedef typename numeric<T>::parametric_function Function;

            ////////////////////////////////////////////////////////////////////
            //
            // Sample interface
            //
            ////////////////////////////////////////////////////////////////////
            class SampleType : public counted_object
            {
            public:
                FitVariables variables;

                inline virtual ~SampleType() throw() {}
                inline explicit SampleType() : variables() {}
                virtual T computeD2(Function &F, const Array &params) const throw() = 0;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SampleType);
            };

            class Sample : public SampleType
            {
            public:
                inline explicit Sample(const Array &userX, const Array &userY, Array &userZ) :
                X(userX), Y(userY), Z(userZ)
                {
                }

                const Array &X;
                const Array &Y;
                Array       &Z;


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Sample);
            };


        };

    }
}

#endif

