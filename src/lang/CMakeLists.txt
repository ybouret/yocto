PROJECT(lang)

FILE(GLOB decl yocto/lang/*.hpp)
FILE(GLOB impl yocto/lang/*.cpp)
SET(srcs ${decl} ${impl})
SOURCE_GROUP(lang FILES ${srcs})
INSTALL(FILES ${decl} DESTINATION include/yocto/lang)

FILE(GLOB pattern_decl yocto/lang/pattern/*.hpp)
FILE(GLOB pattern_impl yocto/lang/pattern/*.cpp)
SET(pattern_srcs ${pattern_decl} ${pattern_impl})
SOURCE_GROUP(lang/pattern FILES ${pattern_srcs})
INSTALL(FILES ${pattern/decl} DESTINATION include/yocto/lang/pattern)

INCLUDE_DIRECTORIES(. ../yocto)

ADD_LIBRARY(y-lang STATIC ${srcs} ${pattern_srcs})

INSTALL( TARGETS y-lang ARCHIVE DESTINATION lib )


IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
