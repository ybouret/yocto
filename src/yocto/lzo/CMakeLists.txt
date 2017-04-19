PROJECT(y-lzo)

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

IF(YOCTO_AUTO_TEST)

IF(NOT MSVC)

ADD_EXECUTABLE(test-lzo testmini.c)
TARGET_LINK_LIBRARIES(test-lzo y-lzo)

ADD_CUSTOM_COMMAND( 
	TARGET  test-lzo
	POST_BUILD
	COMMAND test-lzo
	COMMENT "Auto Testing LZO..."
)

ENDIF(NOT MSVC)

ENDIF(YOCTO_AUTO_TEST)


