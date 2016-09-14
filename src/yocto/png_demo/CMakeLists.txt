
#FIND_PACKAGE(ZLIB REQUIRED)
INCLUDE_DIRECTORIES(../zlib ${CMAKE_CURRENT_BINARY_DIR}/../zlib)

ADD_LIBRARY(y-png STATIC 
png.c
pngerror.c
pngget.c
pngmem.c
pngpread.c
pngread.c
pngrio.c
pngrtran.c
pngrutil.c
pngset.c
pngtrans.c
pngwio.c
pngwrite.c
pngwtran.c
pngwutil.c
)

ADD_EXECUTABLE(pngtest pngtest.c)
TARGET_LINK_LIBRARIES(pngtest y-png y-z)
IF(YOCTO_LINUX OR YOCTO_FREEBSD)
	TARGET_LINK_LIBRARIES(pngtest m)
ENDIF()
