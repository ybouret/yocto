PROJECT(fame)

FILE( GLOB decl yocto/fame/*.hpp )
FILE( GLOB impl yocto/fame/*.cpp )
SET( srcs ${decl} ${impl} )


INSTALL( FILES ${decl} DESTINATION include/yocto/fame )
SOURCE_GROUP( fame FILES ${srcs} )

FILE( GLOB mesh_decl yocto/fame/mesh/*.hpp )
FILE( GLOB mesh_impl yocto/fame/mesh/*.cpp )
SET( mesh_srcs ${mesh_decl} ${mesh_impl} )
SOURCE_GROUP( fame/mesh FILES ${mesh_srcs} )
INSTALL( FILES ${mesh_decl} DESTINATION include/yocto/fame/mesh )

FILE( GLOB format_decl yocto/fame/format/*.hpp )
FILE( GLOB format_impl yocto/fame/format/*.cpp )
SET( format_srcs ${format_decl} ${format_impl} )
SOURCE_GROUP( fame/format FILES ${format_srcs} )
INSTALL( FILES ${format_decl} DESTINATION include/yocto/fame/format )



INCLUDE_DIRECTORIES( . ../yocto ../mk ../gfx ../mpi)

ADD_LIBRARY(y-fame STATIC 
${srcs}
${mesh_srcs}
${format_srcs}
)

INSTALL( TARGETS y-fame ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
