#ifndef YOCTO_LUA_STATE_INCLUDED
#define YOCTO_LUA_STATE_INCLUDED 1

#include "yocto/lua/lua.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/string.hpp"

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

            void DumpStack();
            void DumpTable(int index , const char *name = NULL);

            //! double, int, string, bool
            template <typename T>
            T Get(const string &name);

            bool Has(const string &name);
            inline
            bool Hash(const char *name)
            {
                const string __name(name);
                return Has(__name);
            }

            //! wrapper
            template <typename T>
            inline T Get(const char *name) { const string Name(name); return Get<T>(Name); }
            

            template <typename T>
            void Push( typename type_traits<T>::parameter_type arg);

            template <typename T>
            T To(const int idx);

            //! assuming a table on the stack
            template <typename T>
            inline void SetField(const string &name, typename type_traits<T>::parameter_type value)
            {
                assert(lua_istable(L,-1));
                (void)lua_pushlstring(L,name.c_str(),name.length());
                Push<T>(value);
                lua_rawset(L, -3);
            }

            //! wrapper
            template <typename T>
            inline void SetField(const char *name,typename type_traits<T>::parameter_type value)
            {
                const string Name(name);
                SetField<T>(Name,value);
            }

            //! assuming a table (as array) is on the stack
            template <typename T>
            inline void SetEntry(const int idx,typename type_traits<T>::parameter_type value)
            {
                assert(lua_istable(L,-1));
                Push<T>(value);
                lua_rawseti(L,-2,idx);
            }


            //! assuming a table (as array) is on the stack
            template <typename T>
            inline T GetEntry(const int idx)
            {
                assert(lua_istable(L,-1));
                lua_rawgeti(L,-1,idx);
                typename type_traits<T>::const_type ans( To<T>(-1) );
                lua_pop(L,1);
                return ans;
            }

            //! returning the number of items in a table on the stack
            size_t GetTableLength();




            void        DoString(const string &code);
            inline void DoString(const char   *code) { const string Code(code); DoString(Code); }
            void        DoFile(const string &filename);
            inline void DoFile(const char   *filename) { const string fn(filename); DoFile(fn); }



        private:
            lua_State *L;
            YOCTO_DISABLE_COPY_AND_ASSIGN(State);

        public:
            inline int         absindex(int idx) throw() { return lua_absindex(L,idx); }
            inline void        arith(int op) throw() { lua_arith(L,op); }
            inline void        call(int nargs, int nresults) { return lua_call(L,nargs,nresults); }
            inline int         checkstack(int n) { return lua_checkstack(L,n); }
            inline int         compare(int index1, int index2, int op) { return lua_compare(L, index1, index2, op); }
            inline void        concat(int n) { lua_concat(L,n); }
            inline void        copy(int fromidx, int toidx) { lua_copy(L,fromidx,toidx); }
            inline void        createtable(int narr, int nrec) { lua_createtable(L,narr, nrec); }
            inline int         error() { return lua_error(L); }
            inline int         gc(int what,int data) { return lua_gc(L,what,data); }
            inline int         getfield(int index,const char *k) { return lua_getfield(L, index,k); }
            inline int         getglobal(const char *name) { return lua_getglobal(L,name); }
            inline int         geti(int index, lua_Integer i) { return lua_geti(L,index,i); }
            inline int         getmetatable(int index) { return lua_getmetatable(L, index); }
            inline int         gettable(int index) { return lua_gettable(L,index); }
            inline int         gettop() { return lua_gettop(L); }
            inline int         getuservalue(int index) { return lua_getuservalue(L,index); }
            inline void        insert(int index) { lua_insert(L,index); }
            inline int         isboolean(int index) { return lua_isboolean(L,index); }
            inline int         iscfunction(int index) { return lua_iscfunction(L,index); }
            inline int         isfunction(int index) { return lua_isfunction(L,index); }
            inline int         isinteger(int index) { return lua_isinteger(L,index); }
            inline int         islightuserdata(int index) { return lua_islightuserdata(L,index); }
            inline int         isnil(int index) { return lua_isnil(L,index); }
            inline int         isnone(int index) { return lua_isnone(L,index); }
            inline int         isnoneornil(int index) { return lua_isnoneornil(L,index); }
            inline int         isNumber(int index) { return lua_isnumber(L,index); }
            inline int         isstring(int index) { return lua_isstring(L,index); }
            inline int         istable(int index)  { return lua_istable(L,index); }
            inline int         isthread(int index) { return lua_isthread(L,index); }
            inline int         isuserdata(int index) { return lua_isuserdata(L,index); }
            inline int         isyieldable() { return lua_isyieldable(L); }
            inline void        newtable() { lua_newtable(L); }
            inline void *      newuserdata(size_t sz) { return lua_newuserdata(L,sz); }
            inline int         next(int index) { return lua_next(L,index); }
            inline int         numbertointeger(lua_Number n, lua_Integer *p) { return lua_numbertointeger(n,p); }
            inline int         pcall(int nargs, int nresults, int msgh) { return lua_pcall(L,nargs,nresults,msgh); }
            inline void        pop(int n)         { lua_pop(L,n);         }
            inline void        pushboolean(int b) { lua_pushboolean(L,b); }
            inline void        pushcclosure(lua_CFunction fn, int n) { return lua_pushcclosure(L,fn,n); }
            inline void        pushcfunction(lua_CFunction fn) { lua_pushcfunction(L,fn); }
            inline void        pushinteger(lua_Integer n) { lua_pushinteger(L,n); }
            inline void        pushlightuserdata(void *p)  { lua_pushlightuserdata(L,p); }
            inline const char *pushlstring(const char *s, const size_t l) { return lua_pushlstring(L,s,l); }
            inline void        pushnil() { lua_pushnil(L); }
            inline const char *pushstring(const char *s) { return lua_pushstring(L,s); }
            inline void        pushvalue(int index) { return lua_pushvalue(L,index); }
            inline int         rawequal(int index1, int index2) { return lua_rawequal(L, index1,index2); }
            inline int         rawget(int index) { return lua_rawget(L,index); }
            inline int         rawgeti(int index, lua_Integer n) { return lua_rawgeti(L, index, n); }
            inline size_t      rawlen(int index) { return lua_rawlen(L,index); }
            inline void        rawset(int index) {  lua_rawset(L,index); }
            inline void        rawseti(int index,lua_Integer i) {  lua_rawseti(L,index,i); }
            inline void       _register(const char *name, lua_CFunction f) { lua_register(L, name,f); }
            inline void        remove(int index) { return lua_remove(L,index); }
            inline void        replace(int index) { return lua_replace(L,index); }
            inline void        rotate(int idx, int n) { return lua_rotate(L,idx,n); }
            inline void        setfield(int index, const char *k) { lua_setfield(L,index,k); }
            inline void        setglobal(const char *name) { lua_setglobal(L,name); }
            inline void        seti(int index, lua_Integer n) { lua_seti(L,index,n); }
            inline void        settable(int index) { lua_settable(L,index); }
            inline void        settop(int index) { lua_settop(L,index); }
            inline void        setuservalue(int index) { lua_setuservalue(L,index); }
            inline int         status() { return lua_status(L); }
            inline size_t      stringtonumber(const char   *s) { return lua_stringtonumber(L,s); }
            inline size_t      stringtonumber(const string &s) { return lua_stringtonumber(L,s.c_str());}
            inline bool        toboolean(int index) { return 1 == lua_toboolean(L,index); }
            inline int         tointeger(int index) { return int(lua_tointeger(L,index)); }
            inline const char *tostring(int index)  { return lua_tostring(L,index); }
            inline double      tonumber(int index)  { return lua_tonumber(L,index); }
            inline int         type(int index) { return lua_type(L,index); }
            inline const char*_typename(int tp) { return lua_typename(L,tp); }

            inline const char *type_name_at(int index) { return lua_typename(L,lua_type(L,index)); }

        };

    }
}


#endif

