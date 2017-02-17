#include "yocto/alchemy/boot.hpp"


namespace yocto
{

    namespace alchemy
    {
        boot::component:: component(const species::pointer &s, const double w) throw() :
        sp(s),
        weight(w)
        {
        }

        boot::component:: ~component() throw()
        {
        }

        boot::component:: component(const component &other) throw() :
        sp( other.sp ),
        weight( other.weight )
        {
        }

        const string & boot::component:: key() const throw()
        {
            return sp->name;
        }
        

    }

}

namespace yocto
{

    namespace alchemy
    {
        boot::constraint:: constraint(const library::pointer &p,
                                      const double            v) throw() :
        pLib(p),
        value(v)
        {
        }

        boot::constraint:: ~constraint() throw()
        {
        }

        void  boot::constraint:: add(const string &name, const double weight)
        {
            component *pComp = search(name);
            if(pComp)
            {
                pComp->weight += weight;
            }
            else
            {
                const component comp( (*pLib)[name], weight );
                if(!insert(comp))
                {
                    throw exception("unexpected insert failure of '%s' in boot::constraint", name.c_str());
                }
            }
        }

        void  boot::constraint:: add(const char   *name, const double weight)
        {
            const string Name(name);
            add(Name,weight);
        }


        std::ostream & operator<<( std::ostream &os, const boot::constraint &cc)
        {
            os << cc.value << "=";
            for(boot::constraint::const_iterator it = cc.begin(); it != cc.end(); ++it)
            {
                const boot::component &comp = *it;
                const double           w    = comp.weight;
                const string          &id   = comp.key();
                bool  show = true;
                if(w>0)
                {
                    os << '+' << w;
                }
                else
                {
                    if(w<0)
                    {
                        os << w;
                    }
                    else
                    {
                        show = false;
                    }
                }
                if(show)
                {
                    os << "*[" << id << "]";
                }

            }
            return os;
        }


    }
}

namespace yocto
{
    namespace alchemy
    {
        boot:: ~boot() throw()
        {
        }

        boot:: boot(const library::pointer &p) :
        pLib(p),
        constraints(pLib->size(),as_capacity)
        {
            
        }


        void boot:: electroneutrality()
        {
            constraint::pointer cc( new constraint(pLib,0) );
            constraints.push_back(cc);

            for(library::const_iterator it = pLib->begin(); it != pLib->end(); ++it )
            {
                const species::pointer &sp = *it;
                cc->add(sp->name,sp->z);
            }
        }


        void boot:: osmolarity(const double osm)
        {
            constraint::pointer cc( new constraint(pLib,osm) );
            constraints.push_back(cc);

            for(library::const_iterator it = pLib->begin(); it != pLib->end(); ++it )
            {
                const species::pointer &sp = *it;
                cc->add(sp->name,1.0);
            }

        }


        void boot:: conserve(const string &name, const double C)
        {
            constraint::pointer cc( new constraint(pLib,C) );
            cc->add(name,1.0);
            constraints.push_back(cc);
        }


        void boot:: conserve(const string &nameA, const string &nameB, const double C)
        {
            constraint::pointer cc( new constraint(pLib,C) );
            cc->add(nameA,1.0);
            cc->add(nameB,1.0);
            constraints.push_back(cc);
        }


        std::ostream & operator<<(std::ostream &os,const boot &b)
        {
            for(size_t i=1;i<=b.constraints.size();++i)
            {
                os << *b.constraints[i] << std::endl;
            }
            return os;
        }


    }
}


