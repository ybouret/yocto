#ifndef YOCTO_INK_OPS_PARTICLE_INCLUDED
#define YOCTO_INK_OPS_PARTICLE_INCLUDED 1

#include "yocto/ink/parallel.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        
        
        class Particle : public Vertex::List
        {
        public:
            typedef core::list_of_cpp<Particle> List;
            
            size_t    indx;
            Particle *next;
            Particle *prev;
            explicit  Particle(size_t id) throw();
            virtual  ~Particle() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particle);
        };
        
        typedef Pixmap<size_t> ParticleMap;
        
        class Particles : public ParticleMap, public Particle::List
        {
        public:
            explicit Particles(const Bitmap &bmp);
            virtual ~Particles() throw();
            
            template <typename T,typename ZTEST>
            inline void build( const Pixmap<T> &src, ZTEST &ztest)
            {
                
            }
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particles);
            
        };
        
    }
}

#endif

