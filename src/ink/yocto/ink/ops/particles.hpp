#ifndef YOCTO_INK_PARTICLES_INLCUDED
#define YOCTO_INK_PARTICLES_INCLUDED 1

#include "yocto/ink/types.hpp"
namespace yocto
{
    namespace Ink
    {
        class Particle : public Vertex::List
        {
        public:
            typedef core::list_of_cpp<Particle> List;

            const size_t    indx;
            Particle       *next;
            Particle       *prev;

            explicit Particle(const size_t id) throw();
            virtual ~Particle() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particle);
        };

        class Particles: public Particle::List
        {
        public:
            explicit Particles() throw();
            virtual ~Particles() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particles);
        };

    }
}

#endif

