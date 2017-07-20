PROJECT(lang)

FILE(GLOB decl yocto/lang/*.hpp)
FILE(GLOB impl yocto/lang/*.cpp)
SET(srcs ${decl} ${impl})
SOURCE_GROUP(lang FILES ${srcs})
INSTALL(FILES ${decl} DESTINATION include/yocto/lang)

INCLUDE_DIRECTORIES(. ../yocto)

ADD_LIBRARY(y-lang STATIC ${srcs})

INSTALL( TARGETS y-lang ARCHIVE DESTINATION lib )


IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
