PROJECT(visit)

INCLUDE(../../share/visit.cmake)

IF(VISIT_FOUND)

	INCLUDE_DIRECTORIES( . ../yocto ../mpi )

	FILE(GLOB decl yocto/visit/*.hpp)
	FILE(GLOB impl yocto/visit/*.cpp)
	
	SET( srcs ${decl} ${impl} )
	INSTALL( FILES ${decl} DESTINATION include/yocto/visit )
	SOURCE_GROUP( visit FILES ${srcs})
	
	ADD_LIBRARY( y-visit STATIC ${srcs})
	INSTALL( TARGETS y-visit ARCHIVE DESTINATION lib )
	
	IF(YOCTO_BUILD_TESTS)
		ADD_SUBDIRECTORY(tests)
	ENDIF()

ENDIF(VISIT_FOUND)

