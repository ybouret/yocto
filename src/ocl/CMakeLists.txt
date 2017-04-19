PROJECT(ocl)

INCLUDE(../../share/opencl.cmake)

IF(YOCTO_OCL_FOUND)
	FILE(GLOB decl yocto/ocl/*.hpp)
	FILE(GLOB impl yocto/ocl/*.cpp)
	SET(srcs ${decl} ${impl})

	INCLUDE_DIRECTORIES(. ../yocto)
	ADD_LIBRARY( y-ocl STATIC ${srcs} )

	INSTALL( FILES    ${decl}       DESTINATION include/yocto/ocl )
	INSTALL( TARGETS  y-ocl ARCHIVE DESTINATION lib )

ADD_SUBDIRECTORY(tests)

ENDIF(YOCTO_OCL_FOUND)
