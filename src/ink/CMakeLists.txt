PROJECT(ink)

FILE(GLOB decl yocto/ink/*.hpp)
FILE(GLOB impl yocto/ink/*.cpp)
SET(srcs ${impl} ${decl})
INSTALL( FILES ${decl} DESTINATION include/yocto/ink )
SOURCE_GROUP( ink FILES ${srcs} )

INCLUDE_DIRECTORIES(
.
../yocto 
../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/zlib
)


ADD_LIBRARY(y-ink STATIC ${srcs})
TARGET_LINK_LIBRARIES(y-ink y-tiff)

INSTALL( TARGETS y-ink ARCHIVE DESTINATION lib)

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
