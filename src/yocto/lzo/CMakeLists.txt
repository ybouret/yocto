PROJECT(lzo)

SET( decl 
	minilzo.h 
	lzoconf.h 
	lzodefs.h)
	
SET( srcs minilzo.c ${decl} )

INSTALL( FILES ${decl} DESTINATION include/yocto/lzo)

SOURCE_GROUP( lzo FILES ${srcs} )

ADD_LIBRARY(y-lzo STATIC ${srcs})

INSTALL( TARGETS y-lzo ARCHIVE DESTINATION lib )

#SET(YOCTO_AUTO_TEST ON)

ADD_EXECUTABLE(lzotest testmini.c)
TARGET_LINK_LIBRARIES(lzotest y-lzo)


ADD_CUSTOM_TARGET( test-lzo
COMMAND lzotest 
COMMENT "Testing lzo")


