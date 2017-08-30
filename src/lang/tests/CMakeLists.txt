FILE(GLOB srcs test-*.cpp)

ADD_EXECUTABLE(test-lang main.cpp ${srcs})

TARGET_LINK_YOCTO(test-lang lang)
#INSTALL( TARGETS test-lang RUNTIME DESTINATION share/yocto/demo)

ENABLE_TESTING()

ADD_TEST(Module  test-lang module)
ADD_TEST(Source  test-lang source)
#ADD_TEST(Pattern test-lang pattern)
ADD_TEST(POSIX   test-lang posix)
ADD_TEST(Types   test-lang types)
