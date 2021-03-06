
#include "yocto/ink/ops/fft.hpp"

namespace yocto
{

    namespace Ink
    {

        FFT::  FFT() throw() : zpx(0), pfn(0), tpx(0)
        {}
        FFT:: ~FFT() throw() {}

        unit_t FFT::LengthFor(const unit_t L)
        {
            return next_power_of_two<size_t>(L);
        }

        Area FFT::ComputeAreaFor(const Area &sub)
        {
            return Area( 0,0, LengthFor(sub.w), LengthFor(sub.h) );
        }
    }

}

#include "yocto/code/unroll.hpp"

namespace yocto
{
    using namespace math;

    namespace Ink
    {
        static inline
        void run_fft(real_t *data, const size_t nn[],int isign) throw()
        {
            static const size_t ndim=2;
            assert(data);
            assert(nn);
            assert(ndim>0);
            assert(1==isign||-1==isign);

            size_t  i1,i2,i3,i2rev,ip1,ip2,ip3;

            size_t ntot=1;
            for(size_t idim=1;idim<=ndim;++idim)
            {
                assert(is_a_power_of_two(nn[idim]));
                ntot *= nn[idim];
            }

            size_t nprev=1;
            for(size_t idim=ndim;idim>=1;--idim)
            {
                const size_t n     = nn[idim];
                const size_t nrem  = ntot/(n*nprev);
                ip1   = nprev << 1;
                ip2   = ip1*n;
                ip3   = ip2*nrem;
                i2rev = 1;
                for(i2=1;i2<=ip2;i2+=ip1)
                {
                    if(i2<i2rev)
                    {
                        const size_t itop =i2+ip1-2;
                        for(i1=i2;i1<=itop;i1+=2)
                        {
                            for(i3=i1;i3<=ip3;i3+=ip2)
                            {
                                const size_t i3rev=(i2rev+i3)-i2;
                                core::bswap<sizeof(complex<real_t>)>(&data[i3],&data[i3rev]);
                            }
                        }
                    }
                    size_t ibit=ip2 >> 1;
                    while (ibit >= ip1 && i2rev > ibit)
                    {
                        i2rev -= ibit;
                        ibit >>= 1;
                    }
                    i2rev += ibit;
                }

                size_t ifp1=ip1;
                const double sgn_two_pi = 6.28318530717958623199592693708837032318115234375 * isign;
                while (ifp1 < ip2)
                {
                    const size_t ifp2  = ifp1 << 1;
                    const double theta = sgn_two_pi/(ifp2/ip1);
                    double       wtemp = sin(0.5*theta);
                    const double wpr   = -2.0*wtemp*wtemp;
                    double       wpi   = sin(theta);
                    double       wr    = 1;
                    double       wi    = 0;
                    for(i3=1;i3<=ifp1;i3+=ip1)
                    {
                        const size_t itop=i3+ip1-2;
                        for(i1=i3;i1<=itop;i1+=2)
                        {
                            for(i2=i1;i2<=ip3;i2+=ifp2)
                            {
                                const size_t k1   = i2;
                                const size_t k2   = k1+ifp1;
                                const size_t k1p1 = k1+1;
                                const size_t k2p1 = k2+1;
                                const real_t tempr=real_t(wr)*data[k2]  -real_t(wi)*data[k2p1];
                                const real_t tempi=real_t(wr)*data[k2p1]+real_t(wi)*data[k2];
                                data[k2]    = data[k1]-tempr;
                                data[k2p1]  = data[k1p1]-tempi;
                                data[k1]   += tempr;
                                data[k1p1] += tempi;
                            }
                        }
                        wr=(wtemp=wr)*wpr-wi*wpi+wr;
                        wi=wi*wpr+wtemp*wpi+wi;
                    }
                    ifp1=ifp2;
                }
                nprev *= n;
            }


            const real_t coef = real_t(1.0/sqrt( double(ntot) ));
#define _YOCTO_FFT_SCALE(INDEX) data[INDEX] *= coef
            YOCTO_LOOP_FUNC_(ntot<<1,_YOCTO_FFT_SCALE,1);
        }


        void FFT:: apply(PixmapZ &zmap, const int isign) throw()
        {
            assert(zmap.model==Bitmap::MemoryIsGlobal);
            const size_t  nn[4] = { 0, size_t(zmap.h), size_t(zmap.w), 0 };
            real_t       *data  = static_cast<real_t *>(zmap.entry)-1;
            run_fft(data,nn,isign);
        }

    }

}
