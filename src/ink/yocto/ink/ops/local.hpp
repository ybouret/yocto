#ifndef YOCTO_INK_OPS_LOCAL_INCLUDED
#define YOCTO_INK_OPS_LOCAL_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/ink/parallel.hpp"
#include "yocto/code/bmove.hpp"
#include "yocto/sort/ysort.hpp"
#include "yocto/sequence/lw-array.hpp"

namespace yocto
{
    namespace Ink
    {


        class Local
        {
        public:
            static const size_t Left   = 0x01;
            static const size_t Right  = 0x02;
            static const size_t Bottom = 0x04;
            static const size_t Top    = 0x08;
            static const size_t SIZE   = 9;
            static const coord  Position[SIZE];
            static const size_t Flags[SIZE];
            static  size_t      GetPositionIndex(const size_t flag);

            virtual ~Local() throw();
            explicit Local() throw();


            size_t     count;         //!< extracted count
            size_t     flags[SIZE];   //!< extracted size

            template <typename T>
            inline void collect(T               *value,
                                const Pixmap<T> &source,
                                const coord      p0 ) throw()
            {
                count = 0;
                for(size_t k=0;k<SIZE;++k)
                {
                    const coord p = p0 + Position[k];
                    if(source.contains(p))
                    {
                        bmove(value[count],source[p]);
                        flags[count] = Flags[k];
                        ++count;
                    }
                }
            }

            template <typename T> static inline
            T Erode( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = min_of(ans,value[k]);
                }
                return ans;
            }

            template <typename T> static inline
            T Dilate( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                assert(value);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = max_of(ans,value[k]);
                }
                return ans;
            }

            template <typename T> static inline
            T Average( const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                assert(value);
                T ans = value[0];
                for(size_t k=1;k<count;++k)
                {
                    ans = Pixel<T>::Average(value,count);
                }
                return ans;
            }

            template <typename T> static inline
            T Median(const T *value, const size_t *flags, const size_t count ) throw()
            {
                assert(count>0);
                assert(value);
                lw_array<T> arr( (T*)value, count);
                ySort(arr, __compare<T> );
                if(0!=(count&1))
                {
                    // odd
                    return value[count>>1];
                }
                else
                {
                    // even
                    assert(count>=2);
                    return Pixel<T>::Average(&value[(count>>1)-1],2);
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Local);
        };

        

    }
}

#endif

