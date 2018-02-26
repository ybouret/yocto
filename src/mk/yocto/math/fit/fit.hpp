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
                Variables(const Variables &other);
                Variables & operator=( const Variables &other);
                
                Variables & operator()(const string &var_name, const string &var_link);
                Variables & operator()(const string &var_name);
                Variables & operator<<(const string &var_name);


                static void LocalToGlobal(Indices         &indices,
                                          const Variables &local,
                                          const Variables &global);

                //! indice of variable name
                size_t operator[](const string &var_name) const;

                //! max length of name
                size_t getMaxNameLength() const throw();

            private:
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
                mutable T      D2;   //!< last D2
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

                virtual size_t items() const throw() = 0; //!< number of data
                virtual T      SStot() const throw() = 0; //!< total sum of squares
                virtual size_t count() const throw() = 0; //!< number of sub-samples
                virtual T      correlation() const throw() = 0;

                void display(std::ostream &os,
                             const Array  &aorg,
                             const Array  &aerr,
                             const char   *prefix=0) const;

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

                //! number of data
                virtual size_t items() const throw();
                
                //! total sum of squares
                virtual T      SStot() const throw();
                
                //! returns 1
                virtual size_t count() const throw();

                //! coefficient correlation
                virtual T      correlation() const throw();

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

                virtual size_t items() const throw();       //!< sum of items()
                virtual T      SStot() const throw();       //!< sum of squares
                virtual size_t count() const throw();       //!< size()
                virtual T      correlation() const throw(); //!< 0
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Samples);
            };


            ////////////////////////////////////////////////////////////////////
            //
            // Least Squares
            //
            ////////////////////////////////////////////////////////////////////
            template <typename T>
            class LS : public Gradient<T>
            {
            public:
                typedef typename Type<T>::Array    Array;
                typedef typename Type<T>::Function Function;
                typedef typename Type<T>::Vector   Vector;
                typedef typename Type<T>::Matrix   Matrix;
                
                typedef functor<bool,TL2(const SampleType<T> &,const Array&)> Callback;

                explicit LS();
                virtual ~LS() throw();

                //! error if curvature is too singular
                bool run(SampleType<T>    &sample,
                         Function          &F,
                         Array             &aorg,
                         const array<bool> &used,
                         Array             &aerr,
                         Callback          *cb = 0);
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(LS);
                size_t         nvar;
                SampleType<T> *psm;
                Function      *pfn;
                Array         *pa0;
                Vector         atry;   //!< trial value
                Vector         step;   //!< the step
                Matrix         cinv;   //!< inverse curvature
                Vector         atmp;   //!< probe value
                int            p10;    //!< lambda  = 10^p10
                T              lambda; //!< lambda value for inversion

                void compute_lambda() throw();
                bool compute_cinv(const Matrix &curv);
                
            public:
                const int min_p10;
                const int max_p10;
                T         Rsq;     //!< determination coefficient R^2
                size_t    cycle;   //!< current cycle
                bool      verbose; //!< for some debugging
                bool      correct; //!< for minium corrrection, default=false
                
            private:
                T eval1d(const T u);
            };

        }
    }
}

#endif

