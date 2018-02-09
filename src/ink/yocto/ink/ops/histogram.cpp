
#include "yocto/ink/ops/histogram.hpp"
#include "yocto/math/stat/int-hist.hpp"
#include "yocto/sequence/lw-array.hpp"

namespace yocto
{
    namespace Ink
    {

        Histogram:: ~Histogram() throw()
        {
        }

        Histogram:: Histogram() throw() :
        bins(),
        hist(),
        source(0),
        target(0),
        proc(0),
        _level(0),
        _kmode(KeepGEQ),
        _invpx(false)
        {
            uint8_t *b = (uint8_t *)&bins[0];
            for(size_t i=0;i<BINS;++i) b[i] = uint8_t(i);
            clear();
        }

        void Histogram:: clear() throw()
        {
            memset(hist,0,sizeof(hist));
        }

        size_t Histogram:: threshold() const throw()
        {
            const lw_array<uint8_t> __bins( (uint8_t *)&bins[0],BINS);
            const lw_array<count_t> __hist( (count_t *)&hist[0],BINS);
            return math::OtsuThreshold(__bins,__hist);
        }

    }
}

