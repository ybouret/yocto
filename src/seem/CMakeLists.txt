PROJECT(seem)

FILE( GLOB decl yocto/seem/*.hpp )
FILE( GLOB impl yocto/seem/*.cpp )
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/seem )

SET(SEEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem)
SET(SEEM_GRM ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.g)
SET(SEEM_INC ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.inc)
SET(SEEM_DEF ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.def)

ADD_CUSTOM_COMMAND(
OUTPUT  ${SEEM_INC} ${SEEM_DEF}
COMMAND yocto-dynamo ${SEEM_GRM} ${SEEM_DIR}
DEPENDS yocto-dynamo ${SEEM_GRM}
)

SOURCE_GROUP( seem FILES ${srcs} ${SEEM_GRM} ${SEEM_INC} ${SEEM_DEF} )

INCLUDE_DIRECTORIES( . ../yocto ../lang)

ADD_LIBRARY(y-seem STATIC  ${srcs} 
${SEEM_GRM} 
${SEEM_INC} 
${SEEM_DEF})
	
INSTALL( TARGETS y-seem ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
