#include "yocto/chemical/boot.hpp"


namespace yocto
{
    namespace chemical
    {
        boot::component:: ~component() throw()
        {
        }

        boot:: component::component(species     &which,
                                    const double weight) :
        sp( &which ),
        w( weight  )
        {
            assert(sp->refcount()>1);
        }

        const string & boot::component:: key() const throw()
        {
            return sp->name;
        }

    }
}

namespace yocto
{
    namespace chemical
    {
        boot:: constraint:: ~constraint() throw()
        {
        }

        boot:: constraint:: constraint( const double v ) : value(v)
        {
        }

        void boot:: constraint:: add( species &sp, const int w)
        {
            assert( sp.refcount() >= 1 );
            const component::pointer p( new component(sp,w) );
            if(!insert(p))
            {
                throw exception("multiple constraint for '%s'", *(sp.name) );
            }
        }

        double  boot:: constraint:: fill( array<double> &p ) const throw()
        {
            for(const_iterator i=begin(); i!=end();++i)
            {
                const component &c = **i;
                p[c.sp->indx] = c.w;
            }
            return value;
        }

        std::ostream & operator<<( std::ostream  &os, const boot::constraint &c)
        {
            os << c.value << '=' << ' ';
            for( boot::components::const_iterator i=c.begin(); i != c.end(); ++i)
            {
                const boot::component &cc = **i;
                const int              w  = cc.w;
                if(!w) continue;
                if(w>=0)
                {
                    os << '+';
                }
                os << w << '*' << '[' << cc.sp->name << ']';
            }
            return os;
        }

    }
}

namespace yocto
{
    namespace chemical
    {

        boot:: ~boot() throw()
        {
        }

        void boot:: clear() throw()
        {
            residue. release();
            p2.      release();
            U.       release();
            dL.      release();
            V.       release();
            dX.      release();
            errc.    release();
            beta.    release();
            Xtry.    release();
            Xorg.    release();
            Q.       release();
            aP2.     release();
            L.       release();
            P.       release();
            M=N=Nc=0;
            dP2 = 0;
        }

        boot:: boot(const string &id) :
        name(id),
        constraints(),
        M(0),
        N(0),
        Nc(0),
        P(),
        L(),
        aP2(),
        dP2(0),
        Q(),
        Xorg(),
        Xtry(),
        beta(),
        errc(),
        dX(),
        V(),
        dL(),
        U(),
        p2(),
        eqs(0),
        Balance( this, & boot::__Balance ),
        Control( this, & boot::__Control )
        {
        }

        boot::constraint & boot:: create( const double value )
        {
            constraint::pointer p( new constraint(value) );
            constraints.push_back(p);
            return *p;
        }

        void boot:: electroneutrality( library &lib )
        {
            constraint &EN = create(0);
            for( library::iterator i=lib.begin(); i != lib.end(); ++i)
            {
                species &sp = **i;
                EN.add(sp,sp.z);
            }
        }

        void boot:: conserve(const double totalC, species &sp1 )
        {
            constraint &c = create(totalC);
            c.add(sp1,1);
        }

        void boot:: conserve(const double totalC, species &sp1, species &sp2)
        {
            constraint &c = create(totalC);
            c.add(sp1,1);
            c.add(sp2,1);
        }

        void boot:: conserve(const double totalC, species &sp1, species &sp2, species &sp3)
        {
            constraint &c = create(totalC);
            c.add(sp1,1);
            c.add(sp2,1);
            c.add(sp3,1);
        }

        std::ostream & operator<<( std::ostream  &os, const boot &b)
        {
            os << "<boot:" << b.name << ",#constraint=" << b.constraints.size() <<">" << std::endl;
            for(size_t i=1;i<=b.constraints.size();++i)
            {
                os << '\t' << *b.constraints[i] << std::endl;
            }
            os << "<boot:" << b.name << "/>";
            return os;
        }


    }
    
}

