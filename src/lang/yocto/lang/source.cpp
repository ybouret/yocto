#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {
        
        Source:: ~Source() throw()
        {
        }

        Source:: Source( const Module::Handle &hModule ) :
        module(hModule),
        cache()
        {
        }

        Char * Source:: query()
        {
            if(cache.size>0)
            {
                return cache.pop_front();
            }
            else
            {
                return module->get();
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
                // got a module: do we have something ?
                Char *ch = module->get();
                if(0!=ch)
                {
                    // yes
                    cache.push_back(ch);
                    assert(1==cache.size);
                    return cache.head;
                }
                else
                {
                    //no
                    return 0;
                }

            }
        }

        void Source:: prefetch(const size_t nmax)
        {

            Char *ch = 0;
            if(nmax>0)
            {
                for(size_t i=0;i<nmax;++i)
                {
                    ch = module->get();
                    if(!ch) break;
                    cache.push_back(ch);
                }
            }
            else
            {
                while( 0 != (ch=module->get()) )
                {
                    cache.push_back(ch);
                }
            }
        }

        void Source:: forward(const size_t n) throw()
        {
            assert(cache.size>=n);
            for(size_t i=n;i>0;--i)
            {
                delete cache.pop_front();
            }
        }

        size_t Source:: in_cache() const throw()
        {
            return cache.size;
        }


        bool Source:: is_active()
        {
            return 0 != peek();
        }
        
    }
}
