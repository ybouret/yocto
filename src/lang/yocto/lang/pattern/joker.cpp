#include "yocto/lang/pattern/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        Joker:: ~Joker() throw()
        {
            assert(motif);
            delete motif;
            motif=0;
        }

        Joker::Joker(const uint32_t t,const Pattern *p) throw() :
        Pattern(t),
        motif(p)
        {
            assert(motif);
            addr = static_cast<Joker *>(this);
        }

        Joker:: Joker(const Joker &other) :
        Pattern(other),
        motif(other.motif->clone())
        {
            addr = static_cast<Joker *>(this);
        }
        
        void Joker:: optimize() throw()
        {
            motif = Optimize( (Pattern *)motif );
        }

        void Joker:: __savCommon(ios::ostream &fp) const
        {
            motif->save(fp);
        }
        
    }

}

#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace Lang
    {
        Optional:: Optional(const Optional &other):
        Joker(other)
        {
        }


        Optional:: Optional(const Pattern *p) throw() :
        Joker(UUID,p)
        {

        }

        Pattern *Optional:: Create(Pattern *p)
        {
            assert(p);
            auto_ptr<Pattern> guard(p);
            Pattern *opt = new Optional(p);
            guard.forget();
            return opt;
        }

        bool Optional::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            if(motif->match(source,result))
            {
                return true;
            }
            else
            {
                return true;
            }
        }

        void Optional:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp << "[label=\"?\",shape=egg];\n";
            motif->__viz(fp);
            __mark(fp); fp << "->"; motif->__mark(fp); fp << ";\n";
        }

        void Optional:: __sav(ios::ostream &fp) const
        {
            __savCommon(fp);
        }

    }

}

namespace yocto
{
    namespace Lang
    {
        AtLeast:: AtLeast(const Pattern *p,
                          const size_t   n) throw() :
        Joker(UUID,p),
        count(n)
        {

        }

        AtLeast:: AtLeast(const AtLeast &other) :
        Joker(other),
        count(other.count)
        {
        }

        Pattern * AtLeast:: Create( Pattern *p, const size_t n)
        {
            assert(p);
            auto_ptr<Pattern> guard(p);
            Pattern *q = new AtLeast(p,n);
            guard.forget();
            return q;
        }

        bool AtLeast::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            size_t num = 0;
            while(true)
            {
                Token res;
                if(motif->match(source,res))
                {
                    ++num;
                    result.merge_back(res);
                }
                else
                {
                    break;
                }
            }
            
            if(num>=count)
            {
                return true;
            }
            else
            {
                source.store(result);
                return false;
            }
        }


        void AtLeast:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp("[label=\">=%u\",shape=egg];\n",unsigned(count));
            motif->__viz(fp);
            __mark(fp); fp << "->"; motif->__mark(fp); fp << ";\n";
        }

        void AtLeast:: __sav(ios::ostream &fp) const
        {
            fp.emit<uint32_t>(count);
            __savCommon(fp);
        }
    }

}

#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Lang
    {
        Counting:: Counting(const Pattern *p, const size_t nmin, const size_t nmax) throw():
        Joker(UUID,p),
        min_count( min_of(nmin,nmax) ),
        max_count( max_of(nmin,nmax) )
        {
        }

        Counting::Counting(const Counting &other) :
        Joker(other),
        min_count(other.min_count),
        max_count(other.max_count)
        {

        }

        Pattern * Counting:: Create( Pattern *p, const size_t nmin, const size_t nmax)
        {
            assert(p);
            auto_ptr<Pattern> guard(p);
            Pattern *q = new Counting(p,nmin,nmax);
            guard.forget();
            return q;
        }



        bool Counting::match(YOCTO_LANG_PATTERN_MATCH_ARGS) const
        {
            assert(0==result.size);
            size_t num = 0;
            while(true)
            {
                Token res;
                if(motif->match(source,res))
                {
                    ++num;
                    result.merge_back(res);
                }
                else
                {
                    break;
                }
            }

            if(num<min_count||num>max_count)
            {
                return true;
            }
            else
            {
                source.store(result);
                return false;
            }
        }

        void Counting:: __viz(ios::ostream &fp) const
        {
            __mark(fp);
            fp("[label=\"[%u:%u]\",shape=egg];\n",unsigned(min_count),unsigned(max_count));
            motif->__viz(fp);
            __mark(fp); fp << "->"; motif->__mark(fp); fp << ";\n";
        }

        void Counting:: __sav(ios::ostream &fp) const
        {
            fp.emit<uint32_t>(min_count);
            fp.emit<uint32_t>(max_count);
            __savCommon(fp);
        }

    }
}

