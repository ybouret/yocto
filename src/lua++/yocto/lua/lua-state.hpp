#ifndef YOCTO_LUA_STATE_INCLUDED
#define YOCTO_LUA_STATE_INCLUDED 1

#include "yocto/lua/lua.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"

namespace yocto
{
    namespace Lua
    {

        class State : public counted_object
        {
        public:
            typedef arc_ptr<State> Pointer;

            explicit State();
            virtual ~State() throw();

            lua_State * operator*();


        private:
            lua_State *L;
            YOCTO_DISABLE_COPY_AND_ASSIGN(State);
        };

        void DumpStack( lua_State *L );
        void DumpTable( lua_State *L, int index , const char *name = NULL);
    }
}

#endif

