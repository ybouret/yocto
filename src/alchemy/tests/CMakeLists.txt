FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-alchemy main.cpp ${test_srcs})
TARGET_LINK_YOCTO( test-alchemy alchemy)
