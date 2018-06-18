
FILE(GLOB test_srcs test-*.cpp *.hpp)
ADD_EXECUTABLE(test-mpi main.cpp ${test_srcs})

TARGET_LINK_YOCTO(     test-mpi mpi)
TARGET_LINK_LIBRARIES( test-mpi ${MPI_LIBRARIES})
INSTALL( TARGETS test-mpi RUNTIME DESTINATION share/yocto/demo)
