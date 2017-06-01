FILE(GLOB test_srcs test-*.cpp *.hpp)
ADD_EXECUTABLE(test-fame main.cpp ${test_srcs})
TARGET_LINK_YOCTO( test-fame mk gfx fame png z )

ENABLE_TESTING()
ADD_TEST(Types test-fame types)
