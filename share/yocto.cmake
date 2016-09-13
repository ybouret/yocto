########################################################################
##
##
## global parameters
##
##
########################################################################
MESSAGE("")
MESSAGE("===> <yocto>")
SET(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES ON)
SET(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES     ON)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS    ON)

########################################################################
##
##
## Find out Platform for specific flags
##
##
########################################################################
MESSAGE("Platform: ${CMAKE_SYSTEM_NAME}")

STRING(COMPARE EQUAL "Darwin"  ${CMAKE_SYSTEM_NAME} YOCTO_DARWIN)
STRING(COMPARE EQUAL "FreeBSD" ${CMAKE_SYSTEM_NAME} YOCTO_FREEBSD)
STRING(COMPARE EQUAL "Linux"   ${CMAKE_SYSTEM_NAME} YOCTO_LINUX)
STRING(COMPARE EQUAL "SunOS"   ${CMAKE_SYSTEM_NAME} YOCTO_SUNOS)
STRING(COMPARE EQUAL "OpenBSD" ${CMAKE_SYSTEM_NAME} YOCTO_OPENBSD)
STRING(COMPARE EQUAL "Windows" ${CMAKE_SYSTEM_NAME} YOCTO_WINDOWS)

########################################################################
##
##
## Setup compilers and flags
##
##
########################################################################
GET_FILENAME_COMPONENT(CC_NAME ${CMAKE_C_COMPILER} NAME )
MESSAGE("Compiler: ${CC_NAME}")
SET(YOCTO_KNOWN_COMPILER OFF)

SET(COMPILER_VERSION "")


########################################################################
##
## 'gnu'
##
########################################################################
IF( "${CC_NAME}" MATCHES "^gcc.*" )
	SET(YOCTO_FAMILY "gnu")
	SET(YOCTO_GNU    ON)
	SET(YOCTO_KNOWN_COMPILER ON)
	
	SET(COMMON_C_FLAGS        "-Wall -pipe" )
	SET(CMAKE_C_FLAGS_DEBUG   "${COMMON_C_FLAGS} -g -O0" )
	SET(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS} -O2 -DNDEBUG=1 -D_FORTIFY_SOURCE=2")

	SET(COMMON_CXX_FLAGS        "-Wall -pipe -fexceptions -Weffc++" )
	SET(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_CXX_FLAGS} -g -O0" )
	SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -O2 -DNDEBUG=1 -D_FORTIFY_SOURCE=2")
 ENDIF()
 
########################################################################
##
## 'clang'
##
########################################################################
 IF( "${CC_NAME}" MATCHES "^clang.*" )
	SET(YOCTO_FAMILY "clang")
	SET(YOCTO_CLANG  ON)
	SET(YOCTO_KNOWN_COMPILER ON)
	
	SET(COMMON_C_FLAGS        "-Wall -pipe" )
	SET(CMAKE_C_FLAGS_DEBUG   "${COMMON_C_FLAGS} -g -O0" )
	SET(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS} -O2 -DNDEBUG=1 -D_FORTIFY_SOURCE=2")

	SET(COMMON_CXX_FLAGS        "-Wall -pipe -fexceptions -Weffc++" )
	SET(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_CXX_FLAGS} -g -O0" )
	SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -O2 -DNDEBUG=1 -D_FORTIFY_SOURCE=2")
	
ENDIF()

########################################################################
##
## 'intel'
##
########################################################################
IF( "${CC_NAME}" MATCHES "^icc.*" )
	SET(YOCTO_FAMILY "intel")
	SET(YOCTO_CLANG          ON)
	SET(YOCTO_KNOWN_COMPILER ON)
	
	SET(COMMON_C_FLAGS        "-Wall -pipe -wd981" )
	SET(CMAKE_C_FLAGS_DEBUG   "${COMMON_C_FLAGS} -g" )
	SET(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS} -O2 -DNDEBUG=1 -DFORTIFY_SOURCE=2")

	SET(COMMON_CXX_FLAGS        "-Wall -pipe -wd981 -fexceptions" )
	SET(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_CXX_FLAGS} -g" )
	SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -O2 -DNDEBUG=1 -DFORTIFY_SOURCE=2")
  
ENDIF()

########################################################################
##
## 'microsoft'
##
########################################################################
IF( "${CC_NAME}" STREQUAL "cl.exe" )
	SET(YOCTO_FAMILY "microsoft")
	SET(YOCTO_MICROSOFT      ON)
	SET(YOCTO_KNOWN_COMPILER ON)
	
	SET(COMMON_C_FLAGS        "" )
	SET(CMAKE_C_FLAGS_DEBUG   "${COMMON_C_FLAGS}" )
	SET(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS} -Ox -DNDEBUG=1")

	SET(COMMON_CXX_FLAGS        "-EHsc" )
	SET(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_CXX_FLAGS}" )
	SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -Ob2it -DNDEBUG=1")
  
ENDIF()

IF(YOCTO_KNOWN_COMPILER)
	MESSAGE("Family  : ${YOCTO_FAMILY}")
ELSE()
	MESSAGE( FATAL_ERROR "Using '${YOCTO_FAMILY}' Family")
ENDIF()

########################################################################
##
##
## end of yocto.cmake
##
##
########################################################################
MESSAGE("===> </yocto>")
MESSAGE("")
