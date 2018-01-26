#include "yocto/ink/bitmap.hpp"
namespace yocto
{
    namespace Ink
    {
        Bitmap:: ~Bitmap() throw()
        {
            memory::kind<memory::global>::release(prv_data,prv_size);
            if(MemoryIsShared == model )
            {
                assert(shBitmap);
                if(shBitmap->liberate())
                {
                    delete shBitmap;
                }
            }
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
        entry(0),
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
            allocate();
        }

        Bitmap:: Bitmap(void   *user_data,
                        const   unit_t D,
                        const   unit_t W,
                        const   unit_t H,
                        const   unit_t Stride) :
        entry(user_data),
        depth( __check_d(D) ),
        w(     __check_w(W) ),
        h(     __check_h(H) ),
        pitch(w*depth),
        stride(Stride),
        xshift( __xshift(depth) ),
        _rows(0),
        shBitmap(0),
        prv_data(0),
        prv_size(0),
        model( MemoryFromUser )
        {
            if(Stride<pitch) throw exception("Bitmap(invalid Stride=%d/Pitch=%d)",int(Stride), int(pitch));
            if(!entry)       throw exception("Bitmap(no user data)");
            allocate_rows_only();
        }

        Bitmap:: Bitmap(const Bitmap &other) :
        entry(0),
        depth( other.depth ),
        w( other.w ),
        h( other.h ),
        pitch(other.pitch),
        stride(other.stride),
        xshift(other.xshift),
        _rows(0),
        shBitmap(other.shBitmap),
        prv_data(0),
        prv_size(0),
        model(other.model)
        {
            switch(model)
            {
                case MemoryIsGlobal:
                    allocate();
                    copy(other);
                    break;

                case MemoryIsShared:
                    assert(shBitmap);
                    shBitmap->withhold();
                    entry = shBitmap->entry;
                    _rows = shBitmap->_rows;
                    break;

                case MemoryFromUser:
                    entry = other.entry;
                    allocate_rows_only();
                    break;
            }
        }

        static inline void *__entry_for(const Bitmap &other, const Rectangle &rect)
        {
            if(!other.contains(rect))
                throw exception("Bitmap does not contain rectangle");
            return (void*)other.get(rect.x,rect.y);
        }

        Bitmap::Bitmap( const Bitmap &other, const Rectangle &rect) :
        entry( __entry_for(other,rect) ),
        depth(other.depth),
        w(rect.w),
        h(rect.h),
        pitch(w*depth),
        stride(other.stride),
        xshift(other.xshift),
        _rows(0),
        shBitmap(other.shBitmap),
        prv_data(0),
        prv_size(0),
        model(other.model)
        {
            switch(model)
            {
                case MemoryIsGlobal:
                    allocate();
                    for(unit_t j=0;j<h;++j)
                    {
                        const void *src = other.get(rect.x,rect.y+j);
                        void       *tgt = this->get(0,j);
                        memcpy(tgt,src,pitch);
                    }
                    break;

                case MemoryIsShared:
                    assert(shBitmap);
                    shBitmap->withhold();
                case MemoryFromUser:
                    allocate_rows_only();
                    break;
            }
        }


        static inline Bitmap *__check_shared(Bitmap *shared)
        {
            if(!shared) throw exception("Bitmap(shared=NULL)");
            return shared;
        }

        Bitmap:: Bitmap( Bitmap *shared ) :
        entry( __check_shared(shared)->entry ),
        depth( shared->depth ),
        w( shared->w ),
        h( shared->h ),
        pitch(  shared->pitch ),
        stride( shared->stride ),
        xshift( shared->xshift ),
        _rows(  shared->_rows ),
        shBitmap(shared),
        prv_data(0),
        prv_size(0),
        model( MemoryIsShared )
        {
            shared->withhold();
        }




        void Bitmap:: allocate()
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


        void Bitmap:: allocate_rows_only()
        {
            prv_size = sizeof(__Row) * h;
            prv_data = memory::kind<memory::global>::acquire(prv_size);
            _rows    = prv_data;
            link_rows();
        }

        void Bitmap:: copy(const Bitmap &other) throw()
        {
            assert(depth==other.depth);
            assert(w==other.w);
            assert(h==other.h);
            assert(pitch==other.pitch);
            assert(_rows);
            assert(other._rows);

             __Row      *self = static_cast<__Row       *>(_rows);
            const __Row *peer = static_cast<const __Row *>(other._rows);
            const size_t ncpy = pitch;
            for(unit_t j=0;j<h;++j)
            {
                assert(self[j].p);
                assert(peer[j].p);
                memcpy(self[j].p,peer[j].p,ncpy);
            }
        }

        void Bitmap:: link_rows() throw()
        {
            assert(_rows);
            assert(entry);
            __Row   *r = static_cast<__Row   *>(_rows);
            uint8_t *p = static_cast<uint8_t *>(entry);
            for(unit_t j=0;j<h;++j,p+=stride)
            {
                r[j].p = p;
                r[j].w = w;
            }
        }

        void Bitmap:: ldz() throw()
        {
            assert(_rows);
            __Row   *r = static_cast<__Row   *>(_rows);
            const size_t nz = pitch;
            for(unit_t j=0;j<h;++j)
            {
                assert(r[j].p);
                memset(r[j].p,0,nz);
            }
        }

        void * Bitmap:: get(const unit_t x, const unit_t y) throw()
        {
            assert(x>=0);assert(x<w);
            assert(y>=0);assert(y<h);
            assert(_rows);
            assert(xshift);
            return xshift((static_cast<__Row   *>(_rows)+y)->p,x);
        }

        const void * Bitmap:: get(const unit_t x, const unit_t y) const throw()
        {
            assert(x>=0);assert(x<w);
            assert(y>=0);assert(y<h);
            assert(_rows);
            assert(xshift);
            return xshift( (static_cast<__Row   *>(_rows)+y)->p,x);
        }

        void Bitmap:: fill_with(const void *data) throw()
        {
            assert(data);
            __Row   *r = static_cast<__Row   *>(_rows);
            for(unit_t j=0;j<h;++j)
            {
                assert(r[j].p);
                uint8_t *p = static_cast<uint8_t *>(r[j].p);
                for(unit_t i=0;i<w;++i,p+=depth)
                {
                    memcpy(p,data,depth);
                }
            }
        }

        void Bitmap:: flip_vertical()   throw()
        {
            const unit_t jmax = h/2;
            __Row   *r = static_cast<__Row   *>(_rows);
            for(unit_t j=0,k=h;j<jmax;++j)
            {
                --k;
                uint8_t *rk = static_cast<uint8_t *>( (r+k)->p );
                uint8_t *rj = static_cast<uint8_t *>( (r+j)->p );

                for(unit_t i=0;i<pitch;++i)
                {
                    cswap(rk[i],rj[i]);
                }
            }
        }

        bool Bitmap:: contains(const unit_t x, const unit_t y) const throw()
        {
            return (x>=0) && (y>=0) && (x<w) && (y<h);
        }

        bool Bitmap:: contains(const Rectangle &rect) const throw()
        {
            return contains(rect.x,rect.y) && contains(rect.x_end,rect.y_end);
        }
    }
}
