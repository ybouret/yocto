PROJECT(chemical)

#core
FILE( GLOB decl yocto/chemical/*.hpp)
FILE( GLOB impl yocto/chemical/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/chemical )
SOURCE_GROUP( chemical FILES ${srcs} )

#eqs
FILE( GLOB eqs_srcs yocto/chemical/eqs/*.cpp)
SOURCE_GROUP( chemical/eqs FILES ${eqs_srcs} )

#eqs
FILE( GLOB lua_srcs yocto/chemical/lua/*.cpp)
SOURCE_GROUP( chemical/lua FILES ${lua_srcs} )


INCLUDE_DIRECTORIES( . .. ../yocto ../mk ../lua++ )




#specific
ADD_LIBRARY( y-chemical STATIC ${srcs} ${eqs_srcs} ${lua_srcs} )
INSTALL( TARGETS y-chemical ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
