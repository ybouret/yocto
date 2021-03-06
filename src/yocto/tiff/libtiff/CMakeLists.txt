PROJECT(y-tiff)

# CMake build for libtiff
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

# Generate headers
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tif_config.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/tif_config.h
               @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tiffconf.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h
               @ONLY)

extra_dist(
  SConstruct
  tif_config.h-vms
  tif_config.vc.h
  tif_config.wince.h
  tiffconf.vc.h
  tiffconf.wince.h
  libtiff.def
  libtiff.map
  libtiffxx.map)

set(tiff_HEADERS
  tiff.h
  tiffio.h
  tiffvers.h)

set(tiff_noinst_HEADERS
  t4.h
  tif_dir.h
  tif_predict.h
  tiffiop.h
  uvcode.h)

set(nodist_tiff_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h)

set(tiff_SOURCES
  tif_aux.c
  tif_close.c
  tif_codec.c
  tif_color.c
  tif_compress.c
  tif_dir.c
  tif_dirinfo.c
  tif_dirread.c
  tif_dirwrite.c
  tif_dumpmode.c
  tif_error.c
  tif_extension.c
  tif_fax3.c
  tif_fax3sm.c
  tif_flush.c
  tif_getimage.c
  tif_jbig.c
  tif_jpeg.c
  tif_jpeg_12.c
  tif_luv.c
  tif_lzma.c
  tif_lzw.c
  tif_next.c
  tif_ojpeg.c
  tif_open.c
  tif_packbits.c
  tif_pixarlog.c
  tif_predict.c
  tif_print.c
  tif_read.c
  tif_strip.c
  tif_swab.c
  tif_thunder.c
  tif_tile.c
  tif_version.c
  tif_warning.c
  tif_write.c
  tif_zip.c)

set(tiffxx_HEADERS
  tiffio.hxx)

set(tiffxx_SOURCES
  tif_stream.cxx)

if(win32_io)
  extra_dist(tif_unix.c)
  list(APPEND tiff_SOURCES tif_win32.c)
else()
  extra_dist(tif_win32.c)
  list(APPEND tiff_SOURCES tif_unix.c)
endif()

MESSAGE(STATUS "TIFF_INCLUDES=${TIFF_INCLUDES}" )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${TIFF_INCLUDES})

add_library(y-tiff STATIC ${tiff_SOURCES} ${tiff_HEADERS} ${nodist_tiff_HEADERS}
            ${tiff_port_SOURCES})


install(TARGETS y-tiff ARCHIVE DESTINATION lib)
install(FILES ${tiff_HEADERS} ${nodist_tiff_HEADERS}
        DESTINATION include/yocto/tiff)
        
IF(OFF)
install(TARGETS tiff
        RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES ${tiff_HEADERS} ${nodist_tiff_HEADERS}
        DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
ENDIF()
