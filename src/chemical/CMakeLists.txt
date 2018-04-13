PROJECT(chemical)
#core
FILE( GLOB decl yocto/chemical/*.hpp)
FILE( GLOB impl yocto/chemical/*.cpp)
SET( srcs ${decl} ${impl} )
INSTALL( FILES ${decl} DESTINATION include/yocto/chemical )
SOURCE_GROUP( chemical FILES ${srcs} )

INCLUDE_DIRECTORIES( . .. ../yocto ../mk ../lua++ )



#specific
ADD_LIBRARY( y-chemical STATIC ${srcs})
INSTALL( TARGETS y-chemical ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
