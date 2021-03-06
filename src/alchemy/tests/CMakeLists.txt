FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-alchemy main.cpp ${test_srcs}
acetic.lua
carbonic.lua
water.lua
phosphoric.lua
boot-ab.lua
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
ADD_TEST(Variables   test-alchemy var)
ADD_TEST(Titration   test-alchemy titration)
ADD_TEST(Acetic      test-alchemy lua ${HERE}/acetic.lua)
ADD_TEST(Carbonic    test-alchemy lua ${HERE}/carbonic.lua)
ADD_TEST(Water       test-alchemy lua ${HERE}/water.lua)
ADD_TEST(Phosphoric  test-alchemy lua ${HERE}/phosphoric.lua)
ADD_TEST(Effectors   test-alchemy eff ${HERE}/acetic.lua)
