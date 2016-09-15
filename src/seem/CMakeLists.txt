FILE( GLOB decl yocto/seem/*.hpp )
FILE( GLOB impl yocto/seem/*.cpp )
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/seem )

SET(SEEM_GRAM ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.g)
SET(SEEM_INCL ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.inc)
SET(SEEM_DEFS ${CMAKE_CURRENT_SOURCE_DIR}/yocto/seem/seem.def)

ADD_CUSTOM_COMMAND(
OUTPUT ${SEEM_INCL}
COMMAND yocto_file2data ${SEEM_GRAM} ${SEEM_INCL}
DEPENDS yocto_file2data ${SEEM_GRAM}
)

ADD_CUSTOM_COMMAND(
OUTPUT ${SEEM_DEFS}
COMMAND yocto_gram2defs SEEM_ ${SEEM_GRAM} ${SEEM_DEFS}
DEPENDS yocto_gram2defs ${SEEM_GRAM})

SOURCE_GROUP( seem FILES ${srcs} ${SEEM_GRAM} ${SEEM_INCL} ${SEEM_DEFS} )

INCLUDE_DIRECTORIES( . ../yocto ../lingua)

ADD_LIBRARY(y-seem STATIC  ${srcs} 
${SEEM_GRAM} 
${SEEM_INCL} 
${SEEM_DEFS})
	
INSTALL( TARGETS y-seem ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()