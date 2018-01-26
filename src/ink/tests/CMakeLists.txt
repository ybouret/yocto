FILE(GLOB test_srcs test-*.cpp)

ADD_EXECUTABLE(test-ink main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-ink ink)

ENABLE_TESTING()
ADD_TEST(Types  test-ink types)
ADD_TEST(Bimap  test-ink bitmap)
ADD_TEST(Pixmap test-ink pixmap)

