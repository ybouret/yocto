#ifndef YOCTO_GFX_OPS_DCT_INCLUDED
#define YOCTO_GFX_OPS_DCT_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/ipso/field2d.hpp"
#include <cstring>

namespace yocto
{
    namespace Ink
    {

        //______________________________________________________________________
        //
        //! Discrete Cosine Transform Interface
        //______________________________________________________________________
        class DCT :
        public Pixmap<double>
        {
        public:
            typedef ipso::field2D<double> Table;
            typedef ipso::patch2D         Patch;

            virtual ~DCT() throw();
            Pixmap<double> pix; //!< where to store reverse double data
            virtual void reverse() throw() = 0;


        protected:
            explicit DCT(const unit_t W, const unit_t H);
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(DCT);
        };


        //______________________________________________________________________
        //
        //! Common (a.k.a) rectangular DCT
        //______________________________________________________________________
        class CommonDCT : public DCT
        {
        public:

            virtual ~CommonDCT() throw();
            explicit CommonDCT(const unit_t W, const unit_t H);

            virtual void reverse() throw();

            template <typename T>
            void forward(const Pixmap<T> &src,
                         const unit_t     xx,
                         const unit_t     yy) throw()
            {
                Pixmap<double> &self  = *this;
                const unit_t    N     = w;
                const unit_t    M     = h;
                
                for(unit_t j=0;j<M;++j)
                {
                    for(unit_t i=0;i<N;++i)
                    {
                        double       q   = 0;
                        const double wij = LAMBDA[j][i];
                        for(unit_t y=0;y<M;++y)
                        {
                            const double Cyj    = YCOS[y][j];
                            for(unit_t x=0;x<N;++x)
                            {
                                const double Cxi = XCOS[x][i];
                                const double wxy = Cxi*Cyj;
                                q += double( src[yy+y][xx+x] ) * wxy;
                            }
                        }
                        
                        self[j][i] = q * wij;
                    }
                }
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(CommonDCT);
            Table        XCOS;
            Table        YCOS;
            Table        LAMBDA; //! warning: Lambda[j][i] !!!
        };
        

        //______________________________________________________________________
        //
        //! Square DCT
        //______________________________________________________________________
        class SquareDCT : public DCT
        {
        public:
            virtual ~SquareDCT() throw();
            explicit SquareDCT(const unit_t W);


            virtual void reverse() throw();

            template <typename T>
            void forward(const Pixmap<T> &src,
                         const unit_t     xx,
                         const unit_t     yy) throw()
            {
                Pixmap<double> &self  = *this;
                const unit_t    N     = w;

                for(unit_t j=0;j<N;++j)
                {
                    for(unit_t i=0;i<N;++i)
                    {
                        double       q   = 0;
                        const double wij = LAMBDA[j][i];
                        for(unit_t y=0;y<N;++y)
                        {
                            const double Cyj    = COS[y][j];
                            for(unit_t x=0;x<N;++x)
                            {
                                const double Cxi = COS[x][i];
                                const double wxy = Cxi*Cyj;
                                q += double( src[yy+y][xx+x] ) * wxy;
                            }
                        }
                        self[j][i] = q * wij;
                    }
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(SquareDCT);
            Table       COS;
            Table       LAMBDA;
        };


        

    }
}

#endif
