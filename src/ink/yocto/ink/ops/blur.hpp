#ifndef YOCTO_INK_OPS_BLUR_INCLUDED
#define YOCTO_INK_OPS_BLUR_INCLUDED 1

#include "yocto/ink/parallel.hpp"

namespace yocto
{
    namespace Ink
    {

        class Blur
        {
        public:
            static unit_t GetLength(const float sig) throw();

            const float  sigma;
            const float  sigm2;
            const float  scale; //! 1.0f/(sig2+sig2)
            const unit_t length;
            explicit Blur(const float sig);
            virtual ~Blur() throw();

            float operator()(const float dx) const throw();

        private:
            size_t       wlen;    //!< wlen>=2*length+1
             float      *weight;  //!< from -length to length

            YOCTO_DISABLE_COPY_AND_ASSIGN(Blur);
        };
    }
}

#endif

