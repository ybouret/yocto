PROJECT(z)

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# We add these other definitions here because CheckTypeSize.cmake
# in CMake 2.4.x does not automatically do so and we want
# compatibility with CMake 2.4.x.
if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
endif()
if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
endif()
if(HAVE_STDDEF_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
endif()
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/zconf.h.cmakein
		${CMAKE_CURRENT_BINARY_DIR}/zconf.h @ONLY)

include(zlib_files.cmake)
SET(ZLIB_PUBLIC_HDRS zlib.h  ${CMAKE_CURRENT_BINARY_DIR}/zconf.h)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})
		
add_library(y-z STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
INSTALL( FILES ${ZLIB_PUBLIC_HDRS} DESTINATION include/yocto/zlib )
INSTALL( TARGETS y-z ARCHIVE       DESTINATION lib )
add_executable(z-example test/example.c)
target_link_libraries(z-example y-z)
	
ADD_CUSTOM_TARGET( test-z
COMMAND z-example
COMMENT "Testing zlib")
	
add_executable(minigzip test/minigzip.c)
target_link_libraries(minigzip y-z)
	
#add_executable(z-infcover test/infcover.c)
#target_link_libraries(z-infcover y-z)
