# will use the mpi version...

SET(VISIT_FOUND OFF)
SET(VISIT $ENV{VISIT} )
MESSAGE( STATUS "VISIT=${VISIT}" )

IF( "" STREQUAL "${VISIT}" )
	MESSAGE("No VisIt interface found" )
ELSE()
	MESSAGE("Building VisIt interface" )
	find_package(MPI REQUIRED)
	SET(VISIT_FOUND ON)
	INCLUDE_DIRECTORIES( ${VISIT}/libsim/V2/include )
	LINK_DIRECTORIES(    ${VISIT}/libsim/V2/lib )
	INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
ENDIF()
