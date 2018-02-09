#ifndef YOCTO_INK_PARTICLES_INLCUDED
#define YOCTO_INK_PARTICLES_INCLUDED 1

#include "yocto/ink/types.hpp"
#include "yocto/ink/pixmap.hpp"

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
            static  int CompareBySize(const Particle *lhs, const Particle *rhs,void*) throw();

            template <typename T, typename U, typename FUNC>
            void transfer( Pixmap<T> &target, const Pixmap<U> &source, FUNC &func) const throw()
            {
                for(const Vertex *vtx=head;vtx;vtx=vtx->next)
                {
                    const coord p = vtx->pos;
                    target[p] = func(source[p]);
                }
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particle);
        };

        class Particles: public Particle::List
        {
        public:
            explicit Particles() throw();
            virtual ~Particles() throw();
            void     sortBySize();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Particles);
        };

    }
}

#endif

