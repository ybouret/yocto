#core
FILE( GLOB decl yocto/alchemy/*.hpp)
FILE( GLOB impl yocto/alchemy/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/alchemy )
SOURCE_GROUP( alchemy FILES ${srcs} )

#lua
FILE( GLOB lua_decl yocto/alchemy/lua/*.hpp)
FILE( GLOB lua_impl yocto/alchemy/lua/*.cpp)
SET( lua_srcs ${lua_decl} ${lua_impl} )
INSTALL( FILES ${lua_decl} DESTINATION include/yocto/alchemy/lua )
SOURCE_GROUP( alchemy/lua FILES ${lua_srcs} )

INCLUDE_DIRECTORIES( . .. ../yocto ../mk ../lua++ )



#specific
ADD_LIBRARY( y-alchemy STATIC ${srcs} ${lua_srcs})
INSTALL( TARGETS y-alchemy ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

