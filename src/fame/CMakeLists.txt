PROJECT(fame)

FILE( GLOB decl yocto/fame/*.hpp )
FILE( GLOB impl yocto/fame/*.cpp )
SET( srcs ${decl} ${impl} )


INSTALL( FILES ${decl} DESTINATION include/yocto/spade )
SOURCE_GROUP( fame FILES ${srcs} )

INCLUDE_DIRECTORIES( . ../yocto ../mk ../gfx ../mpi)

ADD_LIBRARY(y-fame STATIC 
	${srcs}
	)
	
INSTALL( TARGETS y-fame ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
