#ifndef YOCTO_MATH_FIT_INCLUDED
#define YOCTO_MATH_FIT_INCLUDED 1

#include "yocto/math/fcn/derivatives.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace math
    {

        typedef vector<size_t> FitIndices;

        //! a Fit varialble, name=local, link=global
        class FitVariable 
        {
        public:
            const string name;
            const string link;
            const size_t indx;
            const string & key() const throw();
            FitVariable(const string &n, const string &l, const size_t idx);
            FitVariable(const FitVariable &);
            ~FitVariable() throw();
            friend std::ostream & operator<<( std::ostream &os, const FitVariable &fv )
            {
                os << "@" << fv.indx << ":'" << fv.name << "'";
                if(fv.name!=fv.link) os << "->'" << fv.link << "'";
                return os;
            }
        private:
            YOCTO_DISABLE_ASSIGN(FitVariable);
        };

        typedef set<string,FitVariable> FitVariablesType;

        class FitVariables : public FitVariablesType
        {
        public:
            explicit FitVariables(const size_t capa=0);
            virtual ~FitVariables() throw();

            FitVariables & operator()(const string &var_name, const string &var_link);
            FitVariables & operator()(const string &var_name);
            FitVariables & operator<<(const string &var_name);


            static void LocalToGlobal(FitIndices         &indices,
                                      const FitVariables &local,
                                      const FitVariables &global);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(FitVariables);
            void check_no_multiple_link(const string &link) const;
        };

#if 0
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
#endif
        
        template <typename T>
        class Fit
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef array<T>                                 Array;
            typedef vector<T>                                Vector;
            typedef matrix<T>                                Matrix;
            typedef typename numeric<T>::parametric_function Function;

            ////////////////////////////////////////////////////////////////////
            //
            // Sample interface
            //
            ////////////////////////////////////////////////////////////////////
            class SampleType : public counted_object
            {
            public:
                FitVariables   variables; //!< 'local' variables
                FitIndices     indices;   //!< global to local indidces
                
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

            class Samples : public SampleType, public SamplesType
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

