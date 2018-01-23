#include "yocto/ink/bitmap.hpp"
namespace yocto
{
    namespace Ink
    {
        Bitmap:: ~Bitmap() throw()
        {
            switch(model)
            {
                case MemoryIsGlobal:
                    assert(entry);
                    assert(allocated>0);
                    memory::kind<memory::global>::release(entry,allocated);
                    break;
            }
        }


        static inline
        unit_t __check_width(unit_t W)
        {
            if(W<=0) throw exception("Bitmap Width=%d",int(W));
            return --W;
        }

        static inline
        unit_t __check_height(unit_t H)
        {
            if(H<=0) throw exception("Bitmap Height=%d",int(H));
            return --H;
        }

        static inline
        unit_t __check_depth(const unit_t D)
        {
            if(D<=0) throw exception("Bitmap Depth=%d",int(D));
            return D;
        }

        namespace
        {
            static inline
            void * shift1(void *addr, const unit_t dx ) throw()
            {
                return static_cast<uint8_t *>(addr)+dx;
            }

            static inline
            void * shift2(void *addr, const unit_t dx ) throw()
            {
                return static_cast<uint16_t *>(addr)+dx;
            }

            static inline
            void * shift3(void *addr, const unit_t dx ) throw()
            {
                return static_cast<uint8_t *>(addr)+(3*dx);
            }

            static inline
            void * shift4(void *addr, const unit_t dx ) throw()
            {
                return static_cast<uint32_t *>(addr)+dx;
            }

            static inline
            void * shift8(void *addr, const unit_t dx ) throw()
            {
                return static_cast<uint64_t *>(addr)+dx;
            }

            static inline
            void * shift16(void *addr, const unit_t dx ) throw()
            {
                return static_cast<point2d<uint64_t> *>(addr)+dx;
            }

        }

#define Y_INK_SHIFT(D) case D: return shift##D
        static inline
        Bitmap::xShiftProc __get_xshift(const unit_t D)
        {
            switch(D)
            {
                    Y_INK_SHIFT(1);
                    Y_INK_SHIFT(2);
                    Y_INK_SHIFT(3);
                    Y_INK_SHIFT(4);
                    Y_INK_SHIFT(8);
                    Y_INK_SHIFT(16);
                default:
                    break;
            }
            throw exception("Bitmap Depth=%d not supported", int(D));
        }

        Bitmap:: Bitmap(const unit_t D,
                        const unit_t W,
                        const unit_t H) :
        Patch(Coord(0,0),Coord(__check_width(W),__check_height(H))),
        entry(0),
        d(__check_depth(D)),
        w(W),
        h(H),
        xshift( __get_xshift(d) ),
        scanline(w*d),
        stride(scanline),
        model(MemoryIsGlobal),
        allocated(scanline*w),
        sharedBMP(0)
        {
            entry = memory::kind<memory::global>::acquire(allocated);
        }

        Bitmap:: Bitmap(const Bitmap &other) :
        Patch(other),
        entry(0),
        d(other.d),
        w(other.w),
        h(other.h),
        xshift(other.xshift),
        scanline(other.scanline),
        stride(other.stride),
        model(other.model),
        allocated(0),
        sharedBMP(0)
        {
            switch(model)
            {
                case MemoryIsGlobal:
                    allocated = items;
                    entry = memory::kind<memory::global>::acquire(allocated);
                    break;
            }
        }


        void * Bitmap:: getLine(const unit_t y) throw()
        {
            assert(y>=0); assert(y<h);
            return static_cast<uint8_t*>(entry) + (y*stride);
        }

        const void * Bitmap:: getLine(const unit_t y) const throw()
        {
            assert(y>=0); assert(y<h);
            return static_cast<const uint8_t*>(entry) + (y*stride);
        }



    }
}
