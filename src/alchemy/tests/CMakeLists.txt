FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-alchemy main.cpp ${test_srcs}
	acetic.lua
)
TARGET_LINK_YOCTO( test-alchemy alchemy lua++)

SET( HERE ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "HERE=${HERE}")

ENABLE_TESTING()
ADD_TEST(Library     test-alchemy library)
ADD_TEST(Equilibrium test-alchemy equilibrium)
ADD_TEST(Equilibria  test-alchemy equilibria)
ADD_TEST(Boot        test-alchemy boot)
ADD_TEST(ODE         test-alchemy ode)
ADD_TEST(Titration   test-alchemy titration)
ADD_TEST(Acetic      test-alchemy lua ${HERE}/acetic.lua)
