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

            //! a  variable, name=local, link=global
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


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Variables);
                void check_no_multiple_link(const string &link) const;
            };

            //! type agnostice information on sample
            class SampleInfo : public counted_object
            {
            public:
                Variables variables;
                Indices   indices;

                explicit SampleInfo(const size_t capa=0);
                virtual ~SampleInfo() throw();

                void link_to(const Variables &global); //!< prepare I/O
                virtual void link() = 0; //!< prepare I/O depending on type

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(SampleInfo);
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
                typedef typename numeric<T>::parametric_function Function;
                typedef array<T>                                 Array;
                typedef vector<T>                                Vector;

                mutable Vector u; //!< variables values

                explicit SampleType(const size_t capa=0);
                virtual ~SampleType() throw();
                virtual T computeD2(Function           &F,
                                    const Array        &a) const  = 0;

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
                typedef typename SampleType<T>::Array    Array;
                typedef typename SampleType<T>::Function Function;
                typedef arc_ptr<Sample>                  Pointer;
                typedef vector<Pointer>                  Collection;

                virtual ~Sample() throw();
                explicit Sample(const Array &userX, const Array &userY, Array &userZ, const size_t capa=0);

                const Array &X;
                const Array &Y;
                Array       &Z;
                virtual void link(); //!< auto link

                //! compute D2, assuming indices are computed by link()
                virtual T    computeD2(Function           &F,
                                       const Array        &a) const ;
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
                typedef typename SampleType<T>::Array    Array;
                typedef typename SampleType<T>::Function Function;
                
                virtual ~Samples() throw();
                explicit Samples(const size_t capa_samples=0, const size_t capa_global=0);

                Sample<T> & add(const Array &userX, const Array &userY, Array &userZ, const size_t capa_local=0);

                virtual void link(); //!< link collection to this->variables

                //! compute D2, assuming indices are computed by link()
                virtual T    computeD2(Function           &F,
                                       const Array        &a) const ;
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Samples);
            };



        }
    }
}

#endif

