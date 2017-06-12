SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS    ON)

SET(PNG_MAJOR 1)
SET(PNG_MINOR 6)
SET(PNG_PATCH 29)

SET(PNG_VERSION ${PNG_MAJOR}.${PNG_MINOR}.${PNG_PATCH})
SET(PNG_TARGET  libpng-${PNG_VERSION})
SET(PNG_ARCHIVE ${PNG_TARGET}.tar.gz)

IF(NOT EXISTS ${PNG_ARCHIVE})
	MESSAGE("Downloading...")
	FILE(DOWNLOAD 
	ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/${PNG_ARCHIVE} 
	${CMAKE_CURRENT_SOURCE_DIR}/${PNG_ARCHIVE}
	SHOW_PROGRESS)
ENDIF()

IF(NOT IS_DIRECTORY ${PNG_TARGET} )
	MESSAGE("Extracting...")
	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E tar xzf ${PNG_ARCHIVE} )
ENDIF()

SET(PNG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sandbox)

IF(NOT EXISTS ${PNG_PATH})
	MESSAGE("Configuring...")
	EXECUTE_PROCESS(COMMAND ./configure 
							--enable-shared=no 
							--prefix=${PNG_PATH}
							--enable-arm-neon=no
							--enable-mips-msa=no
		WORKING_DIRECTORY ${PNG_TARGET} )
		
	MESSAGE("Compiling...")
	EXECUTE_PROCESS(COMMAND make -s install
		WORKING_DIRECTORY ${PNG_TARGET})

ENDIF()

FILE(GLOB PNG_PUBLIC_HEADERS ${PNG_PATH}/include/png*.h)
FILE(GLOB PNG_PRIVATE_HEADERS ${PNG_TARGET}/png*.h)

MESSAGE("Copying public  headers, dumping into png_files.cmake")
FILE( WRITE "png_files.cmake" "SET(PNG_PUBLIC_HEADERS\n" )
FOREACH(f IN ITEMS ${PNG_PUBLIC_HEADERS})
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} .)
	GET_FILENAME_COMPONENT(h ${f} NAME)
	FILE( APPEND "png_files.cmake" "\t${h}\n")
ENDFOREACH(f)
FILE( APPEND "png_files.cmake" ")\n\n")

MESSAGE("Copying private headers, dumping into png_files.cmake")
FILE( APPEND "png_files.cmake" "SET(PNG_PRIVATE_HEADERS\n" )
FOREACH(f IN ITEMS ${PNG_PRIVATE_HEADERS})
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} .)
	GET_FILENAME_COMPONENT(h ${f} NAME)
	FILE( APPEND "png_files.cmake" "\t${h}\n")
ENDFOREACH(f)
FILE( APPEND "png_files.cmake" ")\n\n")

MESSAGE("Finding Object Files...")
EXECUTE_PROCESS(COMMAND ar t ${PNG_PATH}/lib/libpng${PNG_MAJOR}${PNG_MINOR}.a OUTPUT_VARIABLE PNG_RAW_CONTENT)
MESSAGE("Copying sources, dumping into png_files.cmake")
STRING( REGEX MATCHALL "png[a-z]*" PNG_OBJECTS ${PNG_RAW_CONTENT} ) 
FILE( APPEND "png_files.cmake" "SET(PNG_SOURCES\n")
FOREACH(f IN ITEMS ${PNG_OBJECTS})
	SET(s ${f}.c)
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PNG_TARGET}/${s} .)
	FILE( APPEND "png_files.cmake" "\t${s}\n")
ENDFOREACH()
FILE(APPEND "png_files.cmake" ")\n\n")

MESSAGE("Copy files for test...")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PNG_TARGET}/pngtest.c   .)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PNG_TARGET}/pngtest.png .)



