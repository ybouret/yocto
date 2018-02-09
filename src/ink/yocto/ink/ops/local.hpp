#ifndef YOCTO_INK_OPS_LOCAL_INCLUDED
#define YOCTO_INK_OPS_LOCAL_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/parallel.hpp"

namespace yocto
{
    namespace Ink
    {

        class Local
        {
        public:
            static const size_t Left   = 0x01;
            static const size_t Right  = 0x02;
            static const size_t Bottom = 0x04;
            static const size_t Top    = 0x08;
            static const size_t SIZE   = 9;
            static const coord  Position[SIZE];
            static const size_t Flags[SIZE];
            static  size_t      GetPositionIndex(const size_t flag);

            virtual ~Local() throw();
            explicit Local() throw();

        protected:
            size_t   count;         //!< extracted count
            size_t   flags[SIZE];   //!< extracted size

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Local);
        };

        template <typename T>
        class LocalOps : public Local
        {
        public:
            explicit LocalOps() : Local() {}
            virtual ~LocalOps() throw()   {}

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(LocalOps);
        };

    }
}

#endif

