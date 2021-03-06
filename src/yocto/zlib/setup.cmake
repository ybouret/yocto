SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS    ON)

SET(ZLIB_VERSION 1.2.11)
SET(ZLIB_TARGET  zlib-${ZLIB_VERSION})
SET(ZLIB_ARCHIVE ${ZLIB_TARGET}.tar.gz)
SET(ZLIB_URL     http://zlib.net/${ZLIB_ARCHIVE})
MESSAGE("ZLIB_ARCHIVE=${ZLIB_ARCHIVE}")
IF(NOT EXISTS ${ZLIB_ARCHIVE})
	FILE(DOWNLOAD ${ZLIB_URL} ${CMAKE_CURRENT_SOURCE_DIR}/${ZLIB_ARCHIVE} SHOW_PROGRESS)
ENDIF()

IF(NOT IS_DIRECTORY ${ZLIB_TARGET})
	MESSAGE("Extracting...")
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xfz ${ZLIB_ARCHIVE})
ENDIF()

MESSAGE("Copy files...")
INCLUDE(zlib_files.cmake)


FOREACH(f IN ITEMS ${ZLIB_PUBLIC_HDRS} ${ZLIB_CONF} ${ZLIB_PRIVATE_HDRS} ${ZLIB_SRCS} ${ZLIB_EXAMPLES})
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ZLIB_TARGET}/${f} ./${f})
ENDFOREACH(f)

MESSAGE("Cleanup...")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove -f ${ZLIB_ARCHIVE})
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove_directory ${ZLIB_TARGET})
