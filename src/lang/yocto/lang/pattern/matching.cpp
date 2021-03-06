#include "yocto/lang/pattern/matching.hpp"
#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/pattern/basic.hpp"

namespace yocto
{
    namespace Lang
    {

        Matching:: ~Matching() throw() {}

        Matching:: Matching() :
        motif( new Any1() )
        {
            check();
        }

        Matching::Matching(Pattern *p) throw() :
        motif(p)
        {
            check();
        }

        Matching :: Matching(const Matching &other) : 
        motif(other.motif)
        {

        }


        Matching::Matching(const string &expr) :
        motif( RegExp(expr,NULL) )
        {
            check();
        }

        Matching::Matching(const char *expr) :
        motif( RegExp(expr,NULL) )
        {
            check();
        }

        void Matching:: assign(Pattern *p) throw()
        {
            Pattern::Handle tmp(p);
            tmp.swap_with(motif);
            check();
        }

        void Matching:: swap_with(Matching &other) throw()
        {
            motif.swap_with(other.motif);
        }

        void Matching:: check()
        {
            if(motif->allowsEmpty())
            {
                throw exception("Lang.Matching: pattern allows empty match");
            }
        }


        static const char local_module_id[] = "line";

        bool Matching:: wholly_matches(const string &line) const
        {
            Source         source( Module::OpenData(local_module_id,line) );
            Token          tkn;
            if( motif->match(source,tkn) )
            {
                if(source.peek())
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        }

        bool Matching:: partly_matches(const string &line) const
        {
            Source         source( Module::OpenData(local_module_id,line) );
            Token          tkn;

            while(source.peek())
            {
                Token tkn;
                if(motif->match(source,tkn))
                {
                    return true;
                }
                assert(0==tkn.size);
                assert(source.in_cache()>0);
                source.forward(1);
            }
            return false;
        }
        

    }
}


