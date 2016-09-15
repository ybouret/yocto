
SET(LUA_PUB_HEADERS lua.h luaconf.h lualib.h lauxlib.h lua.hpp)
SET(LUA_LIB_SOURCES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c)

#ADD_DEFINITIONS(-DLUA_COMPAT_5_2)
IF(YOCTO_GNU)
	ADD_DEFINITIONS(--std=c99)
ENDIF()

ADD_LIBRARY(y-lua STATIC ${LUA_LIB_SOURCES})
INSTALL( TARGETS y-lua ARCHIVE DESTINATION lib )
INSTALL( FILES ${LUA_PUB_HEADERS} DESTINATION include/yocto/lua )


ADD_EXECUTABLE(yocto_lua lua.c)
TARGET_LINK_LIBRARIES(yocto_lua y-lua)
IF(YOCTO_SUNOS OR YOCTO_LINUX)
	TARGET_LINK_LIBRARIES(yocto_lua m)
ENDIF()

INSTALL( TARGETS yocto_lua RUNTIME DESTINATION bin )

ENABLE_TESTING()
SET(LUA_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
ADD_TEST(Hello   yocto_lua ${LUA_TESTS_DIR}/hello.lua)
ADD_TEST(Bisect  yocto_lua ${LUA_TESTS_DIR}/bisect.lua)
ADD_TEST(Sort    yocto_lua ${LUA_TESTS_DIR}/sort.lua)
ADD_TEST(Sieve   yocto_lua ${LUA_TESTS_DIR}/sieve.lua)
ADD_TEST(Life    yocto_lua ${LUA_TESTS_DIR}/life.lua)
