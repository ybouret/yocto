#ifndef YOCTO_IPSO_COORD_INCLUDED
#define YOCTO_IPSO_COORD_INCLUDED 1

/**
 Interface for Parallel Splitting of Objects
 */

#include "yocto/math/vecops.hpp"
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

        typedef unit_t           coord1D; //!< system integral unit
        typedef point2d<coord1D> coord2D; //!< in 2D...
        typedef point3d<coord1D> coord3D; //!< in 3D
        typedef bool             bool1D;
        typedef point2d<bool>    bool2D;
        typedef point3d<bool>    bool3D;

#define YOCTO_IPSO_DIM_OF(COORDINATE) (sizeof(COORDINATE)/sizeof(coord1D))
        YOCTO_VECOPS(__coord,coord1D,COORD)
        
        inline bool1D __coord2bool(const coord1D &C) throw() { return (C!=0); }
        inline bool2D __coord2bool(const coord2D &C) throw() { return bool2D(C.x!=0,C.y!=0); }
        inline bool3D __coord2bool(const coord3D &C) throw() { return bool3D(C.x!=0,C.y!=0,C.z!=0); }

        //! x,y,z
        const char * __coord_name(const size_t dim) throw();

        template <typename COORD>
        inline void __coord_printf(FILE *fp, const COORD C)
        {
            fprintf(fp,"[%g",double(__coord(C,0)));
            for(size_t i=1;i<YOCTO_IPSO_DIM_OF(COORD);++i)
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
                static const size_t DIM = YOCTO_IPSO_DIM_OF(COORD);
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

