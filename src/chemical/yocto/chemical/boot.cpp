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

        void boot:: constraint:: add( species &sp, const double w)
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
                const double           w  = cc.w;
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

        boot:: boot(const string &id) :
        name(id),
        constraints()
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

        std::ostream & operator<<( std::ostream  &os, const boot &b)
        {
            for(size_t i=1;i<=b.constraints.size();++i)
            {
                os << *b.constraints[i] << std::endl;
            }
            return os;
        }


    }
    
}

