FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-chemical main.cpp ${test_srcs}
	simple.lua
	boot-ab.lua
	phosphoric.lua
)

TARGET_LINK_YOCTO( test-chemical chemical lua++)

SET( HERE ${CMAKE_CURRENT_SOURCE_DIR} )
#MESSAGE( STATUS "HERE=${HERE}")

ENABLE_TESTING()
ADD_TEST(Library   test-chemical library)
ADD_TEST(Equlibria test-chemical eqs)
ADD_TEST(Balance   test-chemical balance)
ADD_TEST(Normalize test-chemical norm)
