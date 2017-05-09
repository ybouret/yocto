#ifndef YOCTO_SPADE_MESH_INCLUDED
#define YOCTO_SPADE_MESH_INCLUDED 1

#include "yocto/spade/types.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    namespace spade
    {

        class Mesh : public object
        {
        public:
            const string name;
            const size_t dimension;

            virtual ~Mesh() throw();

            

        protected:
            explicit Mesh(const string &id, const size_t dim);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Mesh);
        };

    }
}


#endif
