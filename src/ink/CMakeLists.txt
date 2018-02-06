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

FILE(GLOB ramp_decl yocto/ink/color/ramp/*.hpp)
FILE(GLOB ramp_impl yocto/ink/color/ramp/*.cpp)
SET(ramp_srcs ${ramp_impl} ${ramp_decl})
INSTALL( FILES ${ramp_decl} DESTINATION include/yocto/ink/color/ramp )
SOURCE_GROUP( ink/color/ramp FILES ${ramp_srcs} )

FILE(GLOB image_decl yocto/ink/image/*.hpp)
FILE(GLOB image_impl yocto/ink/image/*.cpp)
SET(image_srcs ${image_impl} ${image_decl})
INSTALL( FILES ${image_decl} DESTINATION include/yocto/ink/image )
SOURCE_GROUP( ink/image FILES ${image_srcs} )

FILE(GLOB ops_decl yocto/ink/ops/*.hpp)
FILE(GLOB ops_impl yocto/ink/ops/*.cpp)
SET(ops_srcs ${ops_impl} ${ops_decl})
INSTALL( FILES ${ops_decl} DESTINATION include/yocto/ink/ops )
SOURCE_GROUP( ink/ops FILES ${ops_srcs} )


INCLUDE_DIRECTORIES(
.
..
../yocto 
../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/zlib
)


ADD_LIBRARY(y-ink STATIC ${srcs} ${color_srcs} ${image_srcs} ${ops_srcs} ${ramp_srcs}) 
TARGET_LINK_LIBRARIES(y-ink y-tiff)

INSTALL( TARGETS y-ink ARCHIVE DESTINATION lib)

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF(TRUE)
 ADD_EXECUTABLE(make_ramp ramps/make_ramp.cpp)
 TARGET_LINK_YOCTO(make_ramp)
ENDIF()


