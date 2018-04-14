FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-chemical main.cpp ${test_srcs})

TARGET_LINK_YOCTO( test-chemical chemical mk)

SET( HERE ${CMAKE_CURRENT_SOURCE_DIR} )
#MESSAGE( STATUS "HERE=${HERE}")

ENABLE_TESTING()
ADD_TEST(Library test-chemical library)
