PROJECT(mpi)

INCLUDE(../../share/mpi.cmake)

IF(MPI_FOUND)
  
  FILE( GLOB decl yocto/mpi/*.hpp )
  FILE( GLOB impl yocto/mpi/*.cpp )
  INSTALL( FILES ${decl} DESTINATION include/yocto/mpi )
  
  SET( srcs ${decl} ${impl} )

  INCLUDE_DIRECTORIES( . ../yocto )
	
  ADD_LIBRARY(y-mpi STATIC ${srcs} )
  INSTALL( TARGETS y-mpi ARCHIVE DESTINATION lib )
  
IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()
  
ENDIF()
