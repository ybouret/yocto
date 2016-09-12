SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS    ON)


########################################################################
##
## Finding out Y_BUILDTYPE : default Debug
##
########################################################################
SET(Y_BUILDTYPE_KNOWN   OFF)

IF("" STREQUAL "${Y_BUILDTYPE}")
	MESSAGE("Setting to Debug")
	SET(Y_BUILDTYPE "Debug")
ENDIF()

IF("Debug" STREQUAL "${Y_BUILDTYPE}")
	SET(Y_BUILDTYPE_KNOWN     ON)
ENDIF()

IF("Release" STREQUAL "${Y_BUILDTYPE}")
	SET(Y_BUILDTYPE_KNOWN     ON)
ENDIF()

IF(NOT Y_BUILDTYPE_KNOWN)
	MESSAGE( FATAL_ERROR "Unknown Y_BUILDTYPE=${Y_BUILDTYPE}" )
ENDIF()

########################################################################
##
## Finding out Y_COMPILERS : default gnu
##
########################################################################
SET(Y_COMPILERS_KNOWN OFF)
IF( "" STREQUAL "${Y_COMPILERS}")
	SET(Y_COMPILERS "gnu")
ENDIF()

IF( "${Y_COMPILERS}" STREQUAL "gnu" )
	SET(CC  "gcc${VERSION}")
	SET(CXX "g++${VERSION}")
	SET(Y_COMPILERS_KNOWN ON)
ENDIF()

IF( "${Y_COMPILERS}" STREQUAL "clang" )
	SET(CC  "clang${VERSION}")
	SET(CXX "clang++${VERSION}")
	SET(Y_COMPILERS_KNOWN ON)
ENDIF()


IF( "${Y_COMPILERS}" STREQUAL "intel" )
	SET(CC  "icc")
	SET(CXX "icpc")
	SET(Y_COMPILERS_KNOWN ON)
ENDIF()

IF(NOT Y_COMPILERS_KNOWN)
	MESSAGE( FATAL_ERROR "Unknown Y_COMPILERS=${Y_COMPILERS}")
ENDIF()	

########################################################################
##
## Finding out Y_GENERATOR : default Unix Makefiles
##
########################################################################
SET(Y_GENERATOR_KNOWN OFF)
IF( "" STREQUAL "${Y_GENERATOR}" )
	SET(Y_GENERATOR "Unix Makefiles")
ENDIF()

IF( "${Y_GENERATOR}" STREQUAL "Unix Makefiles" )
	SET(Y_GENERATOR_KNOWN ON)
ENDIF()

IF(NOT Y_GENERATOR_KNOWN)
	MESSAGE( FATAL_ERROR "Unknown Y_GENERATOR=${Y_GENERATOR}")
ENDIF()

########################################################################
##
## prepare the build process
##
########################################################################
SET(Y_BUILDPATH forge)
MESSAGE("Y_GENERATOR=${Y_GENERATOR}")
MESSAGE("Y_BUILDTYPE=${Y_BUILDTYPE}")
MESSAGE("Y_COMPILERS=${Y_COMPILERS}")
MESSAGE("Y_BUILDPATH=${Y_BUILDPATH}")

MESSAGE("Creating Build Path...")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${Y_BUILDPATH})
MESSAGE("Current Dir=${CMAKE_CURRENT_SOURCE_DIR}")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND}
						-G${Y_GENERATOR}
						-DCMAKE_C_COMPILER=${CC} 
						-DCMAKE_CXX_COMPILER=${CXX}
						-DCMAKE_BUILD_TYPE=${Y_BUILDTYPE}
						${CMAKE_CURRENT_SOURCE_DIR}
				WORKING_DIRECTORY ${Y_BUILDPATH})
				
