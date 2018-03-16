#ifndef YOCTO_INK_BLOB_INCLUDED
#define YOCTO_INK_BLOB_INCLUDED 1

#include "yocto/ink/ops/particles.hpp"
#include "yocto/ink/color/named.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/ink/parallel.hpp"
#include "yocto/ink/color/conv.hpp"

namespace yocto
{
    namespace Ink
    {

        class Blob : public Pixmap<size_t>
        {
        public:
            explicit Blob( const Bitmap &bmp );
            virtual ~Blob() throw();

            template <typename T>
            inline void build(const Pixmap<T> &src,
                              Particles       &particles,
                              const bool       connectFull)
            {
                //______________________________________________________________
                //
                // main algorithm is sequential
                //______________________________________________________________
                Pixmap<size_t>   &self = *this;
                const size_t     nconn  = connectFull ? 8 : 4;
                Vertex::Provider cache;
                Vertex::List     vList;

                YOCTO_INK_AREA_LIMITS(self);
                self.ldz();
                particles.clear();

                //______________________________________________________________
                //
                // loop on every pixel
                //______________________________________________________________
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    Row                           &B_j = self[j];
                    const typename Pixmap<T>::Row &S_j = src[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        //______________________________________________________
                        //
                        //! skip zero pixel
                        //______________________________________________________
                        if( Pixel<T>::IsZero(S_j[i]) )
                        {
                            assert(0==B_j[i]);
                            continue;
                        }

                        //______________________________________________________
                        //
                        //! not a zero pixel
                        //______________________________________________________
                        if(B_j[i]>0)
                        {
                            // already in a blob!
                            continue;
                        }

                        //______________________________________________________
                        //
                        // start a new blob!
                        //______________________________________________________
                        assert(0==vList.size);
                        const size_t iBlob    = particles.size + 1;
                        Particle *   particle = new Particle(iBlob);
                        particles.push_back(particle);

                        //______________________________________________________
                        //
                        // initialize algorithm
                        //______________________________________________________
                        vList.push_back( cache.create(i,j) );
                        while(vList.size>0)
                        {
                            Vertex     *pV = vList.pop_back();
                            const coord p0 = pV->pos;
                            cache.store(pV);

                            // test if we are on a boundary
                            if(Pixel<T>::IsZero( src[p0] ))
                            {
                                continue;
                            }

                            //! check if the point was visited
                            size_t &B = self[p0];
                            if(B<=0)
                            {
                                // add to this
                                self[p0] = iBlob;
                                particle->push_back( new Vertex(p0) );
                                
                                // populate list of not visited neighbours
                                for(size_t k=0;k<nconn;++k)
                                {
                                    const coord p=p0+Core::Shift[k];
                                    if(self.contains(p)&&(0==self[p]))
                                    {
                                        vList.push_back( cache.create(p) );
                                    }
                                }
                            }
                            else
                            {
                                assert(iBlob==B);
                            }

                        }
                    }
                }
                //std::cerr << "cache.size=" << cache.size << std::endl;
                particles.sortBySize();
            }

            //! rewrite numbering
            void rewrite(Particles &particles,size_t indx=0);

            //! compute topology of 1 particle from written particles
            void topology(Particle       *inside,
                          Particle       *border,
                          const Particle &source,
                          const bool      connectFull) const;

            //! compute full topology
            void topology(Particles       *insides,
                          Particles       *borders,
                          const Particles &particles,
                          const bool       connectFull) const;

            //! invert particles and background
            template <typename T> inline
            void invert(Pixmap<T> &target, Engine &engine)
            {
                tgt = &target;
                engine.submit(this, &Blob::invertThread<T>);
            }

            //! keep holes according to current map
            /**
             a hole has all its neighbors zero of self
             sorted in increasing order, normally
             */
            void keepHoles(Particles &holes,const bool connectFull) throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Blob);
            void *tgt;
            template <typename T> inline
            void invertThread(const Domain &dom, threading::context &) throw()
            {
                YOCTO_INK_AREA_LIMITS(dom);
                assert(tgt);
                Pixmap<T> &target = *(Pixmap<T> *)tgt;
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    typename Pixmap<T>::Row   &T_j = target[j];
                    const Pixmap<size_t>::Row &B_j = (*this)[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        const size_t b = B_j[i];
                        if(b>0)
                        {
                            T_j[i] = Pixel<T>::Zero;
                        }
                        else
                        {
                            T_j[i] = Pixel<T>::Opaque;
                        }
                    }
                }

            }
        };

    }
}

#endif

