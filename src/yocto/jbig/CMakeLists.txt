PROJECT(jbig C)

SET(decl jbig.h jbig_ar.h)
SET(impl jbig.c jbig_ar.c)
SET(srcs ${decl} ${impl})
ADD_LIBRARY(y-jbig STATIC ${srcs})

ADD_EXECUTABLE(jbigtest tstcodec.c)
TARGET_LINK_LIBRARIES(jbigtest y-jbig)

ADD_CUSTOM_TARGET( test-jbig 
COMMAND jbigtest 
COMMENT "Testing jbig")


INSTALL( TARGETS y-jbig ARCHIVE DESTINATION lib )
INSTALL( FILES ${decl} DESTINATION include/yocto/jbig)
