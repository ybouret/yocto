#ifndef YOCTO_CHEMICAL_EQUILIBRIUM_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIUM_INCLUDED 1

#include "yocto/chemical/actor.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/math/types.hpp"
#include "yocto/sequence/addr-list.hpp"
#include "yocto/container/tuple.hpp"

namespace yocto
{
    namespace chemical
    {

        typedef core::list_of_cpp<actor>       actors;

        class equilibrium : public counted_object
        {
        public:
            static const int has_none = 0;
            static const int has_fwd  = 0x01;
            static const int has_rev  = 0x02;
            static const int has_both = has_fwd | has_rev;

            YOCTO_QUAD_DECL(STANDARD,range,bool,exists,double,xi,size_t,id,size_t,no);
            inline range() throw() : exists(false), xi(0), id(0),no(0) {}
            inline void clear() throw() { exists=false; xi=0; id=0; no=0; }
            YOCTO_TUPLE_END();

            typedef intr_ptr<string,equilibrium>    pointer;
            typedef set<string,pointer>             database;
            typedef math::numeric<double>::function constant;
            virtual ~equilibrium() throw();

            const string     name;
            const string  &  key() const throw();
            mutable constant K;

            bool operator()(species &sp, const int nu);
            friend std::ostream & operator<<( std::ostream &os, const equilibrium &eq);
            void display( std::ostream &os, const size_t ns) const;
            
            void compile() const;

            //! fill reaction topology
            void fill( array<double> &nu, array<bool> &active) const throw();

            double computeGamma( const array<double> &C, const double Kt ) const throw();
            void   computeGradient( array<double> &Phi, const array<double> &C, const double Kt) const throw();
            
            double computeGammaLHS(const array<double> &C, const double Kt) const throw();
            double computeGammaRHS(const array<double> &C) const throw();

        protected:
            explicit equilibrium(const string &id);
            actors  reactants;
            actors  products;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);
            virtual double getK(double) const= 0;

        public:
            const double nu2;
            const double scale;

            void compute_forward( range &fwd, const array<double> &C) const throw();
            void compute_reverse( range &rev, const array<double> &C) const throw();
            void compute_extents( range &fwd, range &rev, const array<double> &C) const throw();
            bool solve(array<double> &C, const double Kt) const;
        };

        class constant_equilibrium : public equilibrium
        {
        public:
            explicit constant_equilibrium(const string &id, const double Kvalue);
            virtual ~constant_equilibrium() throw();

        private:
            const double __K;
            virtual double getK(double) const;
            YOCTO_DISABLE_COPY_AND_ASSIGN(constant_equilibrium);
        };
        

    }
}

#endif

