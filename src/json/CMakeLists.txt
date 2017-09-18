PROJECT(json)


FILE( GLOB decl yocto/json/*.hpp)
FILE( GLOB impl yocto/json/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/json )
SOURCE_GROUP( json FILES ${srcs} )

SET(JSON_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/yocto/json)
SET(JSON_GRM  ${JSON_DIR}/json.g)
SET(JSON_INC  ${JSON_DIR}/json.inc)
SET(JSON_DEF  ${JSON_DIR}/json.def)

ADD_CUSTOM_COMMAND(
OUTPUT  ${JSON_INC} ${JSON_DEF}
COMMAND yocto-dynamo ${JSON_GRM} ${JSON_DIR}
DEPENDS yocto-dynamo ${JSON_GRM}
)


INCLUDE_DIRECTORIES( . ../yocto ../lang)

ADD_LIBRARY( y-json STATIC ${srcs} ${JSON_GRM} ${JSON_INC} ${JSON_DEF})
INSTALL( TARGETS y-json ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
