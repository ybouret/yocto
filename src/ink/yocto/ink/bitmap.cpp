#include "yocto/ink/bitmap.hpp"
namespace yocto
{
    namespace Ink
    {
        Bitmap:: ~Bitmap() throw()
        {
            switch(type)
            {
                case MemoryIsGlobal:
                    assert(addr);
                    assert(allocated>0);
                    memory::kind<memory::global>::release(addr,allocated);
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
                default:
                    break;
            }
            throw exception("Bitmap Depth=%d not supported", int(D));
        }

        Bitmap:: Bitmap(const unit_t W,
                        const unit_t H,
                        const unit_t D) :
        patch_t(coord_t(0,0),coord_t(__check_width(W),__check_height(H))),
        addr(0),
        d(__check_depth(D)),
        w(W),
        h(H),
        xshift( __get_xshift(d) ),
        scanline(w*d),
        stride(scanline),
        type(MemoryIsGlobal),
        allocated(scanline*w)
        {
            addr = memory::kind<memory::global>::acquire(allocated);
        }



    }
}
