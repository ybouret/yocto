#core
FILE( GLOB decl yocto/alchemy/*.hpp)
FILE( GLOB impl yocto/alchemy/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/chemical )
SOURCE_GROUP( alchemy FILES ${srcs} )

INCLUDE_DIRECTORIES( . .. ../yocto ../mk ../lua++ )

#specific
ADD_LIBRARY( y-alchemy STATIC ${srcs} )
INSTALL( TARGETS y-alchemy ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

