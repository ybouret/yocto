FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-ipso main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-ipso)

ENABLE_TESTING()
ADD_TEST(ipso   test-ipso ipso)
ADD_TEST(divide test-ipso divide)

