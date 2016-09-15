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

# for gnu compatible compiler
SET(YOCTO_COMPILER_VERSION "")
SET(YOCTO_COMPILER_MAJOR "")
SET(YOCTO_COMPILER_MINOR "")
SET(YOCTO_COMPILER_PATCH "")

# function will be call for gnu/clang/intel...
FUNCTION(YOCTO_FIND_COMPILER_VERSION)
	MESSAGE("|_Detecting compiler version...")
	# call cc --version
	EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} --version
					OUTPUT_VARIABLE YOCTO_RAW_COMPILER_VERSION
					RESULT_VARIABLE YOCTO_FIND_COMPILER_VERSION_SUCCESS
					ERROR_QUIET
					)
	IF( NOT ("0" STREQUAL "${YOCTO_FIND_COMPILER_VERSION_SUCCESS}") )
		MESSAGE( FATAL_ERROR "couldn't find compiler version")
	ENDIF()
	# get version pattern
	STRING( REGEX MATCH    "[0-9]+\\.[0-9]+\\.[0-9]+" YOCTO_COMPILER_VERSION "${YOCTO_RAW_COMPILER_VERSION}" )
	STRING( REGEX REPLACE  "([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" YOCTO_COMPILER_MAJOR "${YOCTO_COMPILER_VERSION}")
	STRING( REGEX REPLACE  "[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1" YOCTO_COMPILER_MINOR "${YOCTO_COMPILER_VERSION}")
	STRING( REGEX REPLACE  "[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" YOCTO_COMPILER_PATCH "${YOCTO_COMPILER_VERSION}")
	MESSAGE("|_Found <${YOCTO_COMPILER_VERSION}>")
	MESSAGE("|_major=${YOCTO_COMPILER_MAJOR}, minor=${YOCTO_COMPILER_MINOR}, patchlevel=${YOCTO_COMPILER_PATCH}" )
ENDFUNCTION(YOCTO_FIND_COMPILER_VERSION)

########################################################################
##
## 'gnu'
##
########################################################################
IF( "${CC_NAME}" MATCHES "^gcc.*" )
	SET(YOCTO_FAMILY "gnu")
	SET(YOCTO_GNU    ON)
	SET(YOCTO_KNOWN_COMPILER ON)
	YOCTO_FIND_COMPILER_VERSION()
	
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
	YOCTO_FIND_COMPILER_VERSION()

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
	YOCTO_FIND_COMPILER_VERSION()

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
	
	SET(COMMON_C_FLAGS        "-nologo -D_CRT_SECURE_NO_WARNINGS -wd4996" )
	SET(CMAKE_C_FLAGS_DEBUG   "${COMMON_C_FLAGS}" )
	SET(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS} -Ox -DNDEBUG=1")

	SET(COMMON_CXX_FLAGS        "-nologo -EHsc" )
	SET(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_CXX_FLAGS}" )
	#SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -Ob2it -DNDEBUG=1")
  	SET(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -Ox -DNDEBUG=1")

ENDIF()

IF(YOCTO_KNOWN_COMPILER)
	MESSAGE("Family  : ${YOCTO_FAMILY}")
ELSE()
	MESSAGE( FATAL_ERROR "Using '${YOCTO_FAMILY}' Family")
ENDIF()

########################################################################
##
##
## linking macro
##
##
########################################################################

MACRO(TARGET_LINK_YOCTO tgt)

	SET(ylibs "")

	####################################################################
	# adding libraries and dependencies
	####################################################################
	SET(YOCTO_LD_NET OFF)
	FOREACH( extra ${ARGN} )
			LIST( APPEND ylibs "y-${extra}" )
			
			IF( "json" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-lingua" )
			ENDIF()
			
			IF( "seem" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-lingua" )
			ENDIF()
			
			IF( "png" STREQUAL ${extra} )
				LIST( APPEND ylbis "y-z" )
			ENDIF()
			
			IF( "net" STREQUAL ${extra} )
				SET(YOCTO_LD_NET ON)
			ENDIF()
			
			IF( "bz2++" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-bz2" )
			ENDIF()
			
			IF( "z++" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-z" )
			ENDIF()
			
			IF( "fltk" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-mk" )
			ENDIF()
			
			IF( "tiff" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-jpeg" )
				LIST( APPEND ylibs "y-jbig" )
				LIST( APPEND ylibs "y-z"    )
			ENDIF()
			
			IF( "gfx" STREQUAL ${extra} )
				LIST( APPEND ylibs "y-mk" )
			ENDIF()
			
	ENDFOREACH(extra)
	LIST(APPEND ylibs yocto)
	
	####################################################################
	# specific library flags
	####################################################################
	IF(YOCTO_LINUX OR YOCTO_FREEBSD)
		LIST( APPEND ylibs pthread )
	ENDIF()
	
	IF(YOCTO_LINUX)
		LIST( APPEND ylibs rt )
	ENDIF()
	
	IF(YOCTO_LD_NET)
		IF(WIN32)
			IF(YOCTO_GNU)
			LIST(APPEND ylibs ws2_32)
			ENDIF()
		ENDIF()
		
		IF(YOCTO_SUNOS)
			LIST(APPEND ylibs socket nsl)
		ENDIF()
	ENDIF()
	
	####################################################################
	# apply linking
	####################################################################
	LIST(REMOVE_DUPLICATES ylibs)
	MESSAGE(STATUS "Create ${tgt}: ${ylibs}")
	TARGET_LINK_LIBRARIES(${tgt} ${ylibs})
	
	
	IF( YOCTO_GNU AND WIN32)
		IF("${YOCTO_COMPILER_VERSION}" VERSION_GREATER "4.5.0" )
		TARGET_LINK_LIBRARIES( ${tgt} -static-libgcc -static-libstdc++ )
		ENDIF()
	ENDIF()
	
ENDMACRO(TARGET_LINK_YOCTO)

########################################################################
##
##
## FileToData function
##
##
########################################################################
FUNCTION(YOCTO_FILE2DATA source target)
ADD_CUSTOM_COMMAND(
OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${target}
COMMAND yocto_file2data ${CMAKE_CURRENT_SOURCE_DIR}/${source} ${CMAKE_CURRENT_SOURCE_DIR}/${target}
DEPENDS yocto_file2data ${CMAKE_CURRENT_SOURCE_DIR}/${source}
COMMENT "[File2Data] ${source} => ${target}"
)
ENDFUNCTION(YOCTO_FILE2DATA)

########################################################################
##
##
## end of yocto.cmake
##
##
########################################################################
MESSAGE("===> </yocto>")
