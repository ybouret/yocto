FILE(GLOB srcs test-*.cpp)

ADD_EXECUTABLE(test-lang main.cpp ${srcs})

TARGET_LINK_YOCTO(test-lang lang)
#INSTALL( TARGETS test-lang RUNTIME DESTINATION share/yocto/demo)

ENABLE_TESTING()

#ADD_TEST(token test-lang token)
