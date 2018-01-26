PROJECT(ink)

FILE(GLOB decl yocto/ink/*.hpp)
FILE(GLOB impl yocto/ink/*.cpp)
SET(srcs ${impl} ${decl})
INSTALL( FILES ${decl} DESTINATION include/yocto/ink )
SOURCE_GROUP( ink FILES ${srcs} )

FILE(GLOB color_decl yocto/ink/color/*.hpp)
FILE(GLOB color_impl yocto/ink/color/*.cpp)
SET(color_srcs ${color_impl} ${color_decl})
INSTALL( FILES ${color_decl} DESTINATION include/yocto/ink/color )
SOURCE_GROUP( ink/color FILES ${color_srcs} )


INCLUDE_DIRECTORIES(
.
../yocto 
../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/zlib
)


ADD_LIBRARY(y-ink STATIC ${srcs} ${color_srcs})
TARGET_LINK_LIBRARIES(y-ink y-tiff)

INSTALL( TARGETS y-ink ARCHIVE DESTINATION lib)

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()