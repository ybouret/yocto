#ifndef YOCTO_MATH_FIT_INCLUDED
#define YOCTO_MATH_FIT_INCLUDED 1

#include "yocto/math/fcn/derivatives.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/string.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"

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
            FitVariables & operator=( const FitVariables &other )
            {
                FitVariablesType tmp(other);
                swap_with(tmp);
                return *this;
            }

            template <typename T>
            static void build_matrix(matrix<T>          &J,
                                     const FitVariables &local,
                                     const FitVariables &global);
            template <typename T>
            static void build_vector(vector<T>          &lvec,
                                     const array<T>     &rvec,
                                     const FitVariables &local,
                                     const FitVariables &global);

            //size_t find( const string &key, const char *ctx) const;
            size_t operator[]( const string &key) const;
            size_t operator[]( const char   *key) const;
            
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
                FitVariables   local; //!< local names
                mutable Vector u;     //!< local variables

                virtual ~SampleType() throw();
                explicit SampleType();
                virtual T computeD2(Function           &F,
                                    const Array        &params,
                                    const FitVariables &global) const  = 0;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SampleType);
            };

            ////////////////////////////////////////////////////////////////////
            //
            // Simple Samples
            //
            ////////////////////////////////////////////////////////////////////
            class Sample : public SampleType
            {
            public:
                virtual ~Sample() throw();
                explicit Sample(const Array &userX, const Array &userY, Array &userZ);

                const Array &X;
                const Array &Y;
                Array       &Z;

                virtual T computeD2(Function           &F,
                                    const Array        &params,
                                    const FitVariables &global) const ;
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Sample);
            };

            typedef arc_ptr<Sample>        SamplePointer;
            typedef vector<SamplePointer>  SamplesType;

            class Samples : public SamplesType
            {
            public:
                explicit Samples(const size_t n=0);
                virtual ~Samples() throw();
                virtual T computeD2(Function           &F,
                                    const Array        &params,
                                    const FitVariables &global) const ;

                Sample & add(const Array &userX, const Array &userY, Array &userZ);
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Samples);
            };


        };

    }
}

#endif

