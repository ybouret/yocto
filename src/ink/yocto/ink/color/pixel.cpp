#include "yocto/ink/color/pixel.hpp"

namespace yocto
{

    namespace Ink
    {
        template <> const float Pixel<float>::Zero   = 0.0f;
        template <> const float Pixel<float>::Opaque = 1.0f;
        template <> bool        Pixel<float>::IsZero(const float &x) throw() { return (x<=0.0f); }
        template <> float       Pixel<float>::Inverse(const float &x) throw() { return 1.0f-x; }
        
        template <> float Pixel<float>::Average(const float *value, const size_t count) throw()
        {
            assert(value);
            assert(count>0);
            float sum = value[0];
            for(size_t i=1;i<count;++i)
            {
                sum += value[i];
            }
            return sum/count;
        }

        template <> const double Pixel<double>::Zero   = 0.0;
        template <> const double Pixel<double>::Opaque = 1.0;
        template <> bool         Pixel<double>::IsZero(const double &x) throw() { return (x<=0.0); }
        template <> double       Pixel<double>::Inverse(const double &x) throw() { return 1.0-x; }

        template <> double Pixel<double>::Average(const double *value, const size_t count) throw()
        {
            assert(value);
            assert(count>0);
            double sum = value[0];
            for(size_t i=1;i<count;++i)
            {
                sum += value[i];
            }
            return sum/count;
        }

        template <> const uint8_t Pixel<uint8_t>::Zero   = 0x00;
        template <> const uint8_t Pixel<uint8_t>::Opaque = 0xff;
        template <> bool          Pixel<uint8_t>::IsZero(const uint8_t &x) throw() { return (x<=0); }
        template <> uint8_t       Pixel<uint8_t>::Inverse(const uint8_t &x) throw() { return 0xff-x; }

        template <> uint8_t Pixel<uint8_t>::Average(const uint8_t *value, const size_t count) throw()
        {
            assert(count>0);
            assert(value);
            unsigned sum = value[0];
            for(size_t i=1;i<count;++i)
            {
                sum += unsigned(value[i]);
            }
            return uint8_t(sum/count);
        }

    }

}


