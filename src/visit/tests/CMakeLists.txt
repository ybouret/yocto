
FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-visit main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-visit visit)
TARGET_LINK_LIBRARIES(test-visit simV2 ${MPI_LIBRARIES})

