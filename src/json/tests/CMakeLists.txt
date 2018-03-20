
FILE(GLOB test_srcs test-*.cpp)
FILE(GLOB data_json *.js)
ADD_EXECUTABLE(test-json main.cpp ${test_srcs} ${data_json})
TARGET_LINK_YOCTO( test-json json)

ENABLE_TESTING()
ADD_TEST(Value  test-json value)
ADD_TEST(Load1  test-json load ${CMAKE_CURRENT_SOURCE_DIR}/sample1.js)
ADD_TEST(Load2  test-json load ${CMAKE_CURRENT_SOURCE_DIR}/sample2.js)
ADD_TEST(Load3  test-json load ${CMAKE_CURRENT_SOURCE_DIR}/sample3.js)
ADD_TEST(Load4  test-json load ${CMAKE_CURRENT_SOURCE_DIR}/sample4.js)
ADD_TEST(LoadDB test-json load ${CMAKE_CURRENT_SOURCE_DIR}/db.js)
