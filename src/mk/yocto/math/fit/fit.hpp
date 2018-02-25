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

        namespace Fit
        {
            typedef vector<size_t> Indices;

            ////////////////////////////////////////////////////////////////////
            //
            //! a  variable, name=local, link=global
            //
            ////////////////////////////////////////////////////////////////////
            class Variable
            {
            public:
                const string name;
                const string link;
                const size_t indx;
                const string & key() const throw();
                Variable(const string &n, const string &l, const size_t idx);
                Variable(const Variable &);
                ~Variable() throw();
                friend std::ostream & operator<<( std::ostream &os, const Variable &fv )
                {
                    os << "@" << fv.indx << ":'" << fv.name << "'";
                    if(fv.name!=fv.link) os << "->'" << fv.link << "'";
                    return os;
                }
            private:
                YOCTO_DISABLE_ASSIGN(Variable);
            };

            ////////////////////////////////////////////////////////////////////
            //
            //! set of variables
            //
            ////////////////////////////////////////////////////////////////////
            typedef set<string,Variable> VariablesType;
            class Variables : public VariablesType
            {
            public:
                explicit Variables(const size_t capa=0);
                virtual ~Variables() throw();

                Variables & operator()(const string &var_name, const string &var_link);
                Variables & operator()(const string &var_name);
                Variables & operator<<(const string &var_name);


                static void LocalToGlobal(Indices         &indices,
                                          const Variables &local,
                                          const Variables &global);

                //! indice of variable name
                size_t operator[](const string &var_name) const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Variables);
                void check_no_multiple_link(const string &link) const;
            };

            ////////////////////////////////////////////////////////////////////
            //
            //! type agnostice information on sample
            //
            ////////////////////////////////////////////////////////////////////
            class SampleInfo : public counted_object
            {
            public:
                Variables variables;
                Indices   indices;

                explicit SampleInfo(const size_t capa=0);
                virtual ~SampleInfo() throw();

                virtual void link() = 0; //!< prepare indices and memory

                void link_to(const Variables &global); //!< prepare indices only

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SampleInfo);
            };

            template <typename T>
            struct Type
            {
                typedef array<T>                                        Array;
                typedef functor<T,TL3(T,const Array&,const Variables&)> Function;
                typedef vector<T>                                       Vector;
                typedef typename numeric<T>::parametric_function        ParametricFunction;
                typedef matrix<T>                                       Matrix;
            };

            ////////////////////////////////////////////////////////////////////
            //
            //! defining full fit function type and how to compute its gradient
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            class Gradient : public derivatives<T>
            {
            public:
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Function Function;
                typedef typename Type<T>::Vector   Vector;

                T scale; //!< characteristic, default=1e-4
                explicit Gradient();
                virtual ~Gradient() throw();

                //! compute the gradient with full information, using common 'scale'
                void     compute( Array &dFda, Function &F, const T x, const Array &a, const Variables &v);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Gradient);
                Function        *pfn;
                const Variables *var;
                Vector           dh;

                typename Type<T>::ParametricFunction f;
                T eval_f(const T x, const Array &a);

            };

            ////////////////////////////////////////////////////////////////////
            //
            //! base class for sample operations
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            class SampleType : public SampleInfo
            {
            public:
                typedef typename Type<T>::Function Function;
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Vector   Vector;
                typedef typename Type<T>::Matrix   Matrix;

                mutable Vector u;    //!< variables values
                mutable Vector dFdu; //!< for gradient
                mutable Vector beta; //!< descent direction
                mutable Matrix curv; //!< pseudo curvature

                void allocate(const size_t nvar);

                explicit SampleType(const size_t capa=0);
                virtual ~SampleType() throw();

                //! compute only D2, no gradient, no curvature
                virtual T computeD2(Function           &F,
                                    const Array       &a) const  = 0;

                //! compute D2, gradient and curvatures
                virtual T computeD2(Function           &F,
                                    const Array        &a,
                                    Gradient<T>        &grad) const  = 0;


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SampleType);
            };

            ////////////////////////////////////////////////////////////////////
            //
            // Simple Sample
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            class Sample : public SampleType<T>
            {
            public:
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Function Function;
                typedef arc_ptr<Sample>            Pointer;
                typedef vector<Pointer>            Collection;

                virtual ~Sample() throw();
                explicit Sample(const Array &userX, const Array &userY, Array &userZ, const size_t capa=0);

                const Array &X;
                const Array &Y;
                Array       &Z;
                virtual void link(); //!< auto link

                //! compute D2, assuming indices are computed by link()
                virtual T    computeD2(Function           &F,
                                       const Array        &a) const ;

                //! compute D2, assuming indices are computed and memory allocated by link()
                virtual T computeD2(Function           &F,
                                    const Array        &a,
                                    Gradient<T>        &grad) const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Sample);
                void assign(const Array &a) const;
            };

            ////////////////////////////////////////////////////////////////////
            //
            // Samples collection
            //
            ////////////////////////////////////////////////////////////////////
            template <typename  T>
            class Samples : public SampleType<T>, public Sample<T>::Collection
            {
            public:
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Function Function;
                
                virtual ~Samples() throw();
                explicit Samples(const size_t capa_samples=0, const size_t capa_global=0);

                Sample<T> & add(const Array &userX, const Array &userY, Array &userZ, const size_t capa_local=0);

                virtual void link(); //!< link collection to this->variables

                //! compute D2, assuming indices are computed by link()
                virtual T    computeD2(Function           &F,
                                       const Array        &a) const ;

                //! compute D2, assuming indices are computed and memory allocated by link()
                virtual T computeD2(Function           &F,
                                    const Array        &a,
                                    Gradient<T>        &grad) const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Samples);
            };


            ////////////////////////////////////////////////////////////////////
            //
            // Fit algorithm
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            class LS : public Gradient<T>
            {
            public:
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Function Function;
                typedef typename Type<T>::Vector   Vector;
                typedef functor<bool,TL2(const Sample<T> &,const Array&)> Callback;

                explicit LS();
                virtual ~LS() throw();

                
                bool operator()(Sample<T>         &sample,
                                Function          &F,
                                Array             &aorg,
                                const array<bool> &used,
                                Array             &aerr,
                                Callback          *cb = 0);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(LS);
                size_t     nvar;
                Sample<T> *psm;
                Vector     atry;
            };

        }
    }
}

#endif

