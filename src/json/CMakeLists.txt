PROJECT(json)


FILE( GLOB decl yocto/json/*.hpp)
FILE( GLOB impl yocto/json/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/json )
SOURCE_GROUP( json FILES ${srcs} )

INCLUDE_DIRECTORIES( . ../yocto ../lang)

ADD_LIBRARY( y-json STATIC ${srcs} )
INSTALL( TARGETS y-json ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
