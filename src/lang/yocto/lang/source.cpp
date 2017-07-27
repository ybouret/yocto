#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {
        Source:: hmNode:: hmNode( const Module::Handle &h) throw() :
        next(0),
        hModule(h)
        {
        }

        Source:: hmNode:: ~hmNode() throw()
        {
        }
        
    }

}

namespace yocto
{
    namespace Lang
    {
        Source:: ~Source() throw()
        {
        }

        Source:: Source( const Module::Handle &hModule ) :
        cache(),
        modules()
        {
            modules.store( new hmNode(hModule) );
        }
        
        Char * Source:: query()
        {
            if(cache.size>0)
            {
                return cache.pop_front();
            }
            else
            {
                if(0!=modules.top)
                {
                    return modules.top->hModule->get();
                }
                else
                {
                    return 0;
                }
            }
        }

        void Source:: store( Char *ch ) throw()
        {
            assert(0!=ch);
            cache.push_front(ch);
        }

        void Source:: store(Token &tk) throw()
        {
            while(tk.size>0)
            {
                cache.push_front(tk.pop_back());
            }
        }

        void Source:: store_copy(const Token &tk)
        {
            for(const Char *ch = tk.tail;ch;ch=ch->prev)
            {
                cache.push_front( new Char( *ch ) );
            }
        }

        const Char * Source:: peek()
        {
            if(cache.size>0)
            {
                return cache.head;
            }
            else
            {
                if(0!=modules.top)
                {
                    Char *ch = modules.top->hModule->get();
                    if(0!=ch)
                    {
                        cache.push_back(ch);
                        assert(1==cache.size);
                        return cache.head;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }

            }
        }

        void Source:: prefetch(const size_t nmax)
        {
            if(modules.top)
            {
                Char *ch = 0;
                if(nmax>0)
                {
                    for(size_t i=0;i<nmax;++i)
                    {
                        ch = modules.top->hModule->get();
                        if(!ch) break;
                        cache.push_back(ch);
                    }
                }
                else
                {
                    while( 0 != (ch=modules.top->hModule->get()) )
                    {
                        cache.push_back(ch);
                    }
                }
            }
        }



    }
}
