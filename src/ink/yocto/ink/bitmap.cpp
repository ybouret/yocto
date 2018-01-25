#include "yocto/ink/bitmap.hpp"
namespace yocto
{
    namespace Ink
    {
        Bitmap:: ~Bitmap() throw()
        {
            memory::kind<memory::global>::release(prv_data,prv_size);
            
        }


        static inline unit_t __check_d( const unit_t d )
        {
            if(d<=0) throw exception("Bitmap depth=%d", int(d));
            return d;
        }
        static inline unit_t __check_w( const unit_t w )
        {
            if(w<=0) throw exception("Bitmap width=%d", int(w));
            return w;
        }

        static inline unit_t __check_h( const unit_t h )
        {
            if(h<=0) throw exception("Bitmap height=%d", int(h));
            return h;
        }

        namespace
        {

            static inline void * shift1(void *addr,const unit_t dx) throw()
            {
                return static_cast<uint8_t *>(addr)+dx;
            }

            static inline void * shift2(void *addr,const unit_t dx) throw()
            {
                return static_cast<uint16_t *>(addr)+dx;
            }

            static inline void * shift3(void *addr,const unit_t dx) throw()
            {
                return static_cast<uint8_t *>(addr)+(dx*3);
            }

            static inline void * shift4(void *addr,const unit_t dx) throw()
            {
                return static_cast<uint32_t *>(addr)+dx;
            }

            static inline void * shift8(void *addr,const unit_t dx) throw()
            {
                return static_cast<uint64_t *>(addr)+dx;
            }

            static inline void * shift16(void *addr,const unit_t dx) throw()
            {
                return static_cast< point2d<int64_t> *>(addr)+dx;
            }

#define Y_INK_BITMAP_XSHIFT(DEPTH) case DEPTH: return shift##DEPTH

            Bitmap::XShift __xshift( const int depth )
            {
                switch(depth)
                {
                        Y_INK_BITMAP_XSHIFT(1);
                        Y_INK_BITMAP_XSHIFT(2);
                        Y_INK_BITMAP_XSHIFT(3);
                        Y_INK_BITMAP_XSHIFT(4);
                        Y_INK_BITMAP_XSHIFT(8);
                        Y_INK_BITMAP_XSHIFT(16);
                    default:
                        break;
                }
                throw exception("Bitmap: unsupported depth=%d", depth);
            }
        }


        Bitmap:: Bitmap(const unit_t D,
                        const unit_t W,
                        const unit_t H) :
        depth( __check_d(D) ),
        w(     __check_w(W) ),
        h(     __check_h(H) ),
        pitch(w*depth),
        stride(pitch),
        xshift( __xshift(depth) ),
        _rows(0),
        shBitmap(0),
        prv_data(0),
        prv_size(0),
        model( MemoryIsGlobal )
        {
            const size_t data_offset = 0;
            const size_t data_length = pitch * h;
            const size_t rows_offset = memory::align(data_offset+data_length);
            const size_t rows_length = sizeof(__Row) * h;


            prv_size = memory::align(rows_offset+rows_length);
            prv_data = memory::kind<memory::global>::acquire(prv_size);

            std::cerr <<  w << "x" << h << ":" << depth << std::endl;
            std::cerr << "data.bytes=" << data_length << std::endl;
            std::cerr << "rows.bytes=" << rows_length << std::endl;
            std::cerr << "priv.bytes=" << prv_size    << std::endl;
            uint8_t *p = static_cast<uint8_t *>(prv_data);
            entry = &p[data_offset];
            _rows = &p[rows_offset];

            link_rows();
        }

        void Bitmap:: link_rows() throw()
        {
            assert(_rows);
            assert(entry);
            __Row   *r = static_cast<__Row   *>(_rows);
            uint8_t *p = static_cast<uint8_t *>(entry);
            for(unit_t j=0;j<h;++j,p+=stride,++r)
            {
                r->p = p;
                r->w = w;
            }
        }

        void Bitmap:: ldz() throw()
        {
            assert(_rows);
            __Row   *r = static_cast<__Row   *>(_rows);
            const size_t nz = pitch;
            for(unit_t j=0;j<h;++j)
            {
                assert(r->p);
                memset(r->p,0,nz);
            }
        }

    }
}
