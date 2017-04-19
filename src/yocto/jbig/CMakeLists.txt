PROJECT(y-jbig)

SET(decl jbig.h jbig_ar.h)
SET(impl jbig.c jbig_ar.c)
SET(srcs ${decl} ${impl})
ADD_LIBRARY(y-jbig STATIC ${srcs})

IF(YOCTO_BUILD_TESTS)
	ADD_EXECUTABLE(test-jbig tstcodec.c)
	TARGET_LINK_LIBRARIES(test-jbig y-jbig)
ENDIF()

INSTALL( TARGETS y-jbig ARCHIVE DESTINATION lib )
INSTALL( FILES ${decl} DESTINATION include/yocto/jbig)
