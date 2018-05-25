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
ADD_TEST(Library     test-chemical library)
ADD_TEST(Equilibria  test-chemical eqs)
ADD_TEST(Balance     test-chemical balance)
ADD_TEST(Normalize   test-chemical norm)
ADD_TEST(Boot        test-chemical boot)
ADD_TEST(Lua:BootAB  test-chemical lua ${HERE}/boot-ab.lua)
ADD_TEST(Lua:Simple  test-chemical lua ${HERE}/simple.lua)
ADD_TEST(Lua:PO4     test-chemical lua ${HERE}/phosphoric.lua)

