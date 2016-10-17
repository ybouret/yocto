
SET( VISIT $ENV{VISIT} )
MESSAGE( STATUS "VISIT=${VISIT}" )

IF( "" STREQUAL "${VISIT}" )
	MESSAGE("No VisIt interface found" )
ELSE()
	MESSAGE("Building VisIt interface" )
	INCLUDE_DIRECTORIES( . ../yocto )
	INCLUDE_DIRECTORIES( ${VISIT}/libsim/V2/include )

	FILE(GLOB decl yocto/visit/*.hpp)
	FILE(GLOB impl yocto/visit/*.cpp)
	
	SET( srcs ${decl} ${impl} )
	INSTALL( FILES ${decl} DESTINATION include/yocto/json )
	SOURCE_GROUP( visit FILES ${srcs})
	
	ADD_LIBRARY( y-visit STATIC ${srcs})
	INSTALL( TARGETS y-visit ARCHIVE DESTINATION lib )
	
	IF(YOCTO_BUILD_TESTS)
		ADD_SUBDIRECTORY(tests)
	ENDIF()

ENDIF()

