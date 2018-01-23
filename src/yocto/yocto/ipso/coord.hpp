#ifndef YOCTO_IPSO_COORD_INCLUDED
#define YOCTO_IPSO_COORD_INCLUDED 1

/**
 Interface for Parallel Splitting of Objects
 */

#include "yocto/math/point3d.hpp"
#include "yocto/type/args.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/string.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/memory/pooled.hpp"
#include "yocto/exception.hpp"
#include <cstdio>

namespace yocto
{
    namespace ipso
    {

        typedef unit_t           coord1D;
        typedef point2d<coord1D> coord2D;
        typedef point3d<coord1D> coord3D;

        

        template <typename COORD>
        inline coord1D & __coord(COORD &C,const size_t dim) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D)); return ((coord1D*)&C)[dim];
        }

        template <typename COORD>
        inline const coord1D & __coord(const COORD &C,const size_t dim) throw()
        {
            assert(dim<sizeof(COORD)/sizeof(coord1D)); return ((coord1D*)&C)[dim];
        }

        template <typename COORD>
        inline bool __coord_eq(const COORD &lhs, const COORD &rhs) throw()
        {
            const coord1D  *L = (const coord1D  *)&lhs;
            const coord1D  *R = (const coord1D  *)&rhs;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                if(L[i]!=R[i]) return false;
            }
            return true;
        }

        template <typename COORD>
        inline void __coord_dec(COORD &C) throw()
        {
            coord1D  *q = (coord1D *)&C;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                --q[i];
            }
        }

        template <typename COORD>
        inline COORD __coord_abs(COORD C) throw()
        {
            coord1D  *q = (coord1D *)&C;
            for(size_t i=0;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                const coord1D value = q[i];
                if(value<0) q[i] = -value;
            }
            return C;
        }

        template <typename COORD>
        coord1D __coord_prod(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                p *= q[i];
            }
            return p;
        }

        template <typename COORD> inline
        coord1D __coord_sum(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                p += q[i];
            }
            return p;
        }

        template <typename COORD> inline
        coord1D __coord_max(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                const coord1D qi = q[i];
                if(qi>p) { p=q[i]; }
            }
            return p;
        }

        template <typename COORD> inline
        coord1D __coord_min(COORD C) throw()
        {
            coord1D  *q   = (coord1D *)&C;
            coord1D   p = q[0];
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                const coord1D qi = q[i];
                if(qi<p) { p=q[i]; }
            }
            return p;
        }


        const char * __coord_name(const size_t dim) throw();

        template <typename COORD>
        inline void __coord_printf(FILE *fp, const COORD C)
        {
            fprintf(fp,"[%g",double(__coord(C,0)));
            for(size_t i=1;i<sizeof(COORD)/sizeof(coord1D);++i)
            {
                fprintf(fp," %g",double(__coord(C,i)));
            }
            fprintf(fp,"]'");
        }

        struct __coord_parser
        {
            typedef vector<string,memory::pooled::allocator> strings;
            template <typename COORD>
            static inline COORD get(const char *text, const char *name)
            {
                static const size_t DIM = sizeof(COORD)/sizeof(coord1D);
                const string id   = name;
                const string data = text;
                strings      words(DIM,as_capacity);
                tokenizer::split(words,data,is_sep);
                if(DIM!=words.size())
                {
                    throw exception("_coord_parser%uD: #words=%u", unsigned(DIM), unsigned(words.size()));
                }
                COORD ans;
                coord1D *q = ( (coord1D *)&ans )  - 1;
                for(size_t i=1;i<=DIM;++i)
                {
                    words[i].clean(is_bad);
                    const string label = id + '.' + __coord_name(i-1);
                    q[i] = string2coord(words[i],label);
                }

                return ans;
            }

            static coord1D string2coord(const string &word, const string &label);
            static bool is_sep(char C) throw();
            static bool is_bad(char C) throw();
        };


    }
}

#endif

