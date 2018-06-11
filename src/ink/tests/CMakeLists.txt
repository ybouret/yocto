FILE(GLOB test_srcs test-*.cpp)

ADD_EXECUTABLE(test-ink main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-ink ink png z jpeg tiff)

ENABLE_TESTING()
ADD_TEST(Types   test-ink types)
ADD_TEST(Bimap   test-ink bitmap)
ADD_TEST(Pixmaps test-ink pixmaps)
ADD_TEST(Blur    test-ink blur)
ADD_TEST(DCT     test-ink dct)
ADD_TEST(Draw    test-ink draw)
ADD_TEST(Format  test-ink fmt)
ADD_TEST(Par     test-ink par 1024,768)
ADD_TEST(Stencil test-ink stencil)
ADD_TEST(Stencil test-ink stengrad)

