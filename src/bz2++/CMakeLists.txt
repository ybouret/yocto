PROJECT(bz2++)

FILE( GLOB decl yocto/bz2++/*.hpp)
FILE( GLOB impl yocto/bz2++/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/bz2++ )
SOURCE_GROUP( bz2++ FILES ${srcs} )

INCLUDE_DIRECTORIES( . ../yocto .. )

ADD_LIBRARY( y-bz2++ STATIC ${srcs} )
INSTALL( TARGETS y-bz2++ ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
