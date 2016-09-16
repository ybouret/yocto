
FILE(GLOB test_srcs test-*.cpp *.hpp)
ADD_EXECUTABLE(test-ocl main.cpp ${test_srcs})
TARGET_LINK_YOCTO( test-ocl ocl )
YOCTO_OCL_LINK_TO( test-ocl )

ENABLE_TESTING()


#ADD_CUSTOM_TARGET(check-kr COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS test-kr )
