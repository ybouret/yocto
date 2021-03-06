PROJECT(gfx)

FILE(GLOB decl yocto/gfx/*.hpp)
FILE(GLOB impl yocto/gfx/*.cpp)
SET(srcs ${impl} ${decl})
INSTALL( FILES ${decl} DESTINATION include/yocto/gfx )
SOURCE_GROUP( gfx FILES ${srcs} )

FILE(GLOB color_decl yocto/gfx/color/*.hpp)
FILE(GLOB color_impl yocto/gfx/color/*.cpp)
SET(color_srcs ${color_impl} ${color_decl})
INSTALL( FILES ${color_decl} DESTINATION include/yocto/gfx/color )
SOURCE_GROUP( gfx/color FILES ${color_srcs} )

FILE(GLOB color_ramp_decl yocto/gfx/color/ramp/*.hpp)
FILE(GLOB color_ramp_impl yocto/gfx/color/ramp/*.cpp)
SET(color_ramp_srcs ${color_ramp_impl} ${color_ramp_decl})
INSTALL( FILES ${color_ramp_decl} DESTINATION include/yocto/gfx/color/ramp )
SOURCE_GROUP( gfx/color/ramp FILES ${color_ramp_srcs} )


FILE(GLOB ops_decl yocto/gfx/ops/*.hpp)
FILE(GLOB ops_impl yocto/gfx/ops/*.cpp)
SET(ops_srcs ${ops_impl} ${ops_decl})
INSTALL( FILES ${ops_decl} DESTINATION include/yocto/gfx/ops )
SOURCE_GROUP( gfx/ops FILES ${ops_srcs} )

FILE(GLOB image_decl yocto/gfx/image/*.hpp)
FILE(GLOB image_impl yocto/gfx/image/*.cpp)
SET(image_srcs ${image_impl} ${image_decl})
INSTALL( FILES ${image_decl} DESTINATION include/yocto/gfx/image )
SOURCE_GROUP( gfx/image FILES ${image_srcs} )

FILE(GLOB draw_decl yocto/gfx/draw/*.hpp)
FILE(GLOB draw_impl yocto/gfx/draw/*.cpp)
SET(draw_srcs ${draw_impl} ${draw_decl})
INSTALL( FILES ${draw_decl} DESTINATION include/yocto/gfx/draw )
SOURCE_GROUP( gfx/draw FILES ${draw_srcs} )

INCLUDE_DIRECTORIES( 
. 
..
../yocto 
../mk
../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/tiff/libtiff
${CMAKE_CURRENT_BINARY_DIR}/../yocto/zlib
)


ADD_LIBRARY(y-gfx STATIC ${srcs} ${color_srcs} ${color_ramp_srcs} ${ops_srcs} ${image_srcs} ${draw_srcs} )
TARGET_LINK_LIBRARIES(y-gfx y-tiff)

INSTALL( TARGETS y-gfx ARCHIVE DESTINATION lib)

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF(FALSE)
ADD_EXECUTABLE(yocto-to-jpg yocto-to-jpg.cpp)
TARGET_LINK_YOCTO(yocto-to-jpg gfx tiff png)
ENDIF()

