PROJECT(jpeg C)

SET(LIBSOURCES  jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c 
        jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c 
        jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c 
        jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c 
        jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c 
        jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c 
        jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c 
        jquant2.c jutils.c jmemmgr.c jmemnobs.c)
        
SET(SYSDEPSOURCES jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c)

SET(INSTINCLUDES jerror.h jmorecfg.h jpeglib.h)

SET(OTHERINCLUDES cderror.h cdjpeg.h jdct.h jinclude.h jmemsys.h jpegint.h 
        jversion.h transupp.h)
        
ADD_LIBRARY(y-jpeg STATIC ${LIBSOURCES})

SET(cjpeg_SOURCES     cjpeg.c rdppm.c rdgif.c rdtarga.c rdrle.c rdbmp.c rdswitch.c cdjpeg.c)
SET(djpeg_SOURCES     djpeg.c wrppm.c wrgif.c wrtarga.c wrrle.c wrbmp.c rdcolmap.c cdjpeg.c)
SET(jpegtran_SOURCES  jpegtran.c rdswitch.c cdjpeg.c transupp.c)
SET(rdjpgcom_SOURCES  rdjpgcom.c)
SET(wrjpgcom_SOURCES  wrjpgcom.c)


ADD_EXECUTABLE(cjpeg    ${cjpeg_SOURCES})
ADD_EXECUTABLE(djpeg    ${djpeg_SOURCES})
ADD_EXECUTABLE(jpegtran ${jpegtran_SOURCES})
ADD_EXECUTABLE(rdjpgcom ${rdjpgcom_SOURCES})
ADD_EXECUTABLE(wrjpgcom ${wrjpgcom_SOURCES})

TARGET_LINK_LIBRARIES(cjpeg    y-jpeg)
TARGET_LINK_LIBRARIES(djpeg    y-jpeg)
TARGET_LINK_LIBRARIES(jpegtran y-jpeg)
TARGET_LINK_LIBRARIES(rdjpgcom y-jpeg)
TARGET_LINK_LIBRARIES(wrjpgcom y-jpeg)

SET(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(cmp ${CMAKE_COMMAND} -E compare_files)
ADD_CUSTOM_TARGET( test-jpeg
	COMMAND ${CMAKE_COMMAND} -E remove -f testout.*
	COMMAND djpeg -dct int -ppm -outfile testout.ppm ${SRCDIR}/testorig.jpg
	COMMAND djpeg -dct int -bmp -colors 256 -outfile testout.bmp ${SRCDIR}/testorig.jpg
	COMMAND cjpeg -dct int -outfile testout.jpg  ${SRCDIR}/testimg.ppm
	COMMAND djpeg -dct int -ppm -outfile testoutp.ppm ${SRCDIR}/testprog.jpg
	COMMAND cjpeg -dct int -progressive -opt -outfile testoutp.jpg ${SRCDIR}/testimg.ppm
	COMMAND jpegtran -outfile testoutt.jpg ${SRCDIR}/testprog.jpg
	COMMAND ${cmp} ${SRCDIR}/testimg.ppm  testout.ppm
	COMMAND ${cmp} ${SRCDIR}/testimg.bmp  testout.bmp
	COMMAND ${cmp} ${SRCDIR}/testimg.jpg  testout.jpg
	COMMAND ${cmp} ${SRCDIR}/testimg.ppm  testoutp.ppm
	COMMAND ${cmp} ${SRCDIR}/testimgp.jpg testoutp.jpg
	COMMAND ${cmp} ${SRCDIR}/testorig.jpg testoutt.jpg
	COMMENT "Testing jpeg"
	)
	
