#ifndef YOCTO_MPL_RATIONAL_INCLUDED
#define YOCTO_MPL_RATIONAL_INCLUDED 1

#include "yocto/mpl/integer.hpp"
#include "yocto/exceptions.hpp"
#include <cerrno>

namespace yocto
{
    namespace mpl
    {

        class rational : public object
        {
        public:
            const integer num;
            const natural den;

            virtual ~rational() throw() {}
            inline   rational() : num(0), den(1) {}
            inline   rational(const rational &r) : num(r.num), den(r.den) {}

            inline void xch(rational &r) throw()
            {
                ( (integer &)num ).xch( (integer &)(r.num) );
                ( (natural &)den ).xch( (natural &)(r.den) );
            }
            
            
            inline rational & operator=(const rational &r)
            {
                rational q(r);
                xch(q);
                return *this;
            }

            inline rational(const integer  &x) : num(x), den(1) {}
            inline rational(const integer_t x) : num(x), den(1) {}
            inline rational(const integer  &x, const natural &y) : num(x), den(y) { update(); }
            inline rational(const integer_t x, const word_t   y) : num(x), den(y) { update(); }
            
            inline void update()
            {
                if(den.is_zero()) throw libc::exception(EINVAL,"mpq: zero denominator");
                natural::simplify( (natural &)(num.n), (natural &)(den) );
            }

            inline void ldz() throw()
            {
                ( (integer &)num ).ldz();
                ( (natural &)den ).ld1();
            }
            
            
            inline friend std::ostream & operator<<( std::ostream &os, const rational &r)
            {
                if(r.den<=1)
                {
                    os << r.num;
                }
                else
                {
                    os << '(' << r.num << '/' << r.den << ')';
                }

                return os;
            }

#define YOCTO_MPQ_DECL(OP,OP1,CALL) \
inline friend rational operator OP ( const rational &lhs, const rational &rhs ) \
{ return CALL(lhs,rhs); }\
inline rational & operator OP1(const rational &rhs) { rational r = *this OP rhs; xch(r); return *this; } \
inline friend rational operator OP ( const rational &lhs, const integer &rhs ) \
{ const rational r(rhs); return lhs OP r; } \
inline friend rational operator OP ( const rational &lhs, const integer_t rhs) \
{ const rational r(rhs); return CALL(lhs,r); } \
inline friend rational operator OP ( const integer &lhs, const rational &rhs ) \
{ const rational l(lhs); return CALL(l,rhs); } \
inline friend rational operator OP ( const integer_t lhs,const rational &rhs ) \
{ const rational l(lhs); return CALL(l,rhs); } \
inline rational & operator OP1(const integer  &rhs) { rational r = *this OP rhs; xch(r); return *this; } \
inline rational & operator OP1(const integer_t rhs) { rational r = *this OP rhs; xch(r); return *this; }



            //__________________________________________________________________
            //
            //
            // Addition
            //
            //__________________________________________________________________
            inline static rational add( const rational &lhs, const rational &rhs )
            {
                const integer u = lhs.num * rhs.den;
                const integer v = rhs.num * lhs.den;
                const natural q = lhs.den * rhs.den;
                const integer w = u+v;
                return rational(w,q);
            }
            inline rational operator+() const { return *this; }
            YOCTO_MPQ_DECL(+,+=,add)

            inline rational & inc()
            {
                const rational __one(1);
                return (*this+=__one);
            }

            //! prefix increment
            inline rational & operator++()
            {
                return inc();
            }

            //! postfix increment
            rational   operator++ (int)
            {
                rational sav(*this);
                (void)inc();
                return sav;
            }
            
            //__________________________________________________________________
            //
            //
            // Subtraction
            //
            //__________________________________________________________________
            inline static rational sub( const rational &lhs, const rational &rhs )
            {
                const integer u = lhs.num * rhs.den;
                const integer v = rhs.num * lhs.den;
                const natural q = lhs.den * rhs.den;
                const integer w = u-v;
                return rational(w,q);
            }

            inline rational operator-() const { const integer nn = -num; return rational(nn,den); }
            YOCTO_MPQ_DECL(-,-=,sub)

            inline rational & dec()
            {
                const rational __one(1);
                return (*this-=__one);
            }

            //! prefix increment
            inline rational & operator--()
            {
                return dec();
            }

            //! postfix increment
            rational   operator-- (int)
            {
                rational sav(*this);
                (void)dec();
                return sav;
            }

            //__________________________________________________________________
            //
            //
            // multiplication
            //
            //__________________________________________________________________
            inline static rational mul(const rational &lhs, const rational &rhs )
            {
                const integer u = lhs.num * rhs.num;
                const natural v = lhs.den * rhs.den;
                return rational(u,v);
            }
            YOCTO_MPQ_DECL(*,*=,mul)

            inline static rational sqr( const rational &q )
            {
                const integer __num = integer::sqr( q.num );
                const natural __den = natural::sqr( q.den );
                return rational(__num,__den);
            }

            //__________________________________________________________________
            //
            //
            // division
            //
            //__________________________________________________________________
            inline static rational div(const rational &lhs, const rational &rhs )
            {
                const integer u = lhs.num * rhs.den;
                const integer v = rhs.num * lhs.den;
                (sign_type &)(u.s) = sign_mul(u.s,v.s);
                return rational(u,v.n);
            }
            YOCTO_MPQ_DECL(/,/=,div)

            //__________________________________________________________________
            //
            //
            // comparison
            //
            //__________________________________________________________________
            inline static int compare( const rational &lhs, const rational &rhs )
            {
                const integer u = lhs.num * rhs.den;
                const integer v = rhs.num * lhs.den;
                return integer::compare(
                                        u.s,u.n.ro(),u.n.length(),
                                        v.s,v.n.ro(),v.n.length()
                                        );
            }

#define YOCTO_MPQ_CMP(OP) \
inline friend bool operator OP(const rational &lhs, const rational  &rhs) throw()\
{ return compare(lhs,rhs) OP 0; }

            YOCTO_MPQ_CMP(==)
            YOCTO_MPQ_CMP(!=)
            YOCTO_MPQ_CMP(>=)
            YOCTO_MPQ_CMP(<=)
            YOCTO_MPQ_CMP(>)
            YOCTO_MPQ_CMP(<)

            //__________________________________________________________________
            //
            //
            // approximation
            //
            //__________________________________________________________________
            inline double to_double() const
            {
                switch(num.s)
                {
                    case __zero:
                        break;

                    case __positive:
                        return natural::ratio_of(num.n,den);

                    case __negative:
                        return -natural::ratio_of(num.n,den);
                }
                return 0.0;
            }

            template <typename HASHING_FUNCTION = hashing::sfh >
            class key_hasher
            {
            public:
                HASHING_FUNCTION h;
                inline explicit key_hasher() : h() {}
                inline virtual ~key_hasher() throw() {}
                inline size_t operator()( const rational &q ) throw()
                {
                    h.set();
                    h.run(&q.num.s,sizeof(sign_type) );
                    h.run(q.num.n.ro(),q.num.n.length()  );
                    h.run(q.den.ro(),q.den.length());
                    return h.template key<size_t>();
                }

                inline size_t operator()( const natural &n ) throw()
                {
                    h.set();
                    h.run( n.ro(), n.length() );
                    return h.template key<size_t>();
                }

                inline size_t operator()( const integer &I ) throw()
                {
                    h.set();
                    h.run(&I.s, sizeof(sign_type) );
                    h.run( I.n.ro(), I.n.length() );
                    return h.template key<size_t>();
                }


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(key_hasher);
            };

        };
    }

    typedef mpl::rational mpq;
    template <>
    struct xnumeric<mpq>
    {
        inline static mpq zero()    { return mpq();  }
        inline static mpq one()     { return mpq(1); }
        inline static mpq minimum() { return mpq();  }
    };
    
    namespace math
    {
        inline mpq Fabs(const mpq &r)
        {
            const mpl::integer nn( sign_abs(r.num.s), r.num.n );
            return mpq(nn,r.den);
        }
    }
}

#endif

