PROJECT(spade)

FILE( GLOB decl yocto/spade/*.hpp )
FILE( GLOB impl yocto/spade/*.cpp )
SET( srcs ${decl} ${impl} )


INSTALL( FILES ${decl} DESTINATION include/yocto/spade )
SOURCE_GROUP( spade FILES ${srcs} )

FILE( GLOB format_decl yocto/spade/format/*.hpp )
FILE( GLOB format_impl yocto/spade/format/*.cpp )
SET( format_srcs ${format_decl} ${format_impl} )

INSTALL( FILES ${format_decl} DESTINATION include/yocto/spade/format )
SOURCE_GROUP( spade/format FILES ${format_srcs} )

FILE( GLOB geometry_decl yocto/spade/geometry/*.hpp )
FILE( GLOB geometry_impl yocto/spade/geometry/*.cpp )
SET( geometry_srcs ${geometry_decl} ${geometry_impl} )

INSTALL( FILES ${geometry_decl} DESTINATION include/yocto/spade/geometry )
SOURCE_GROUP( spade/geometry FILES ${geometry_srcs} )

FILE( GLOB mesh_decl yocto/spade/mesh/*.hpp )
FILE( GLOB mesh_impl yocto/spade/mesh/*.cpp )
SET( mesh_srcs ${mesh_decl} ${mesh_impl} )

INSTALL( FILES ${mesh_decl} DESTINATION include/yocto/spade/mesh )
SOURCE_GROUP( spade/mesh FILES ${mesh_srcs} )

INCLUDE_DIRECTORIES( . ../yocto ../mk ../gfx ../mpi)

ADD_LIBRARY(y-spade STATIC 
	${srcs}
	${format_srcs}
	${geometry_srcs}
	${mesh_srcs}
	)
	
INSTALL( TARGETS y-spade ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

