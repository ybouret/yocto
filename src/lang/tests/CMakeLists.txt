FILE(GLOB srcs test-*.cpp)

ADD_EXECUTABLE(test-lang main.cpp ${srcs}
	test.g
	simple.g
	lists.g
	json.g
	calc.g
	seem.g
)

TARGET_LINK_YOCTO(test-lang lang)
#INSTALL( TARGETS test-lang RUNTIME DESTINATION share/yocto/demo)

ADD_CUSTOM_COMMAND(
OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/json.inc
COMMAND yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/json.g ${CMAKE_CURRENT_SOURCE_DIR}
DEPENDS yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/json.g
)

ADD_EXECUTABLE(dyn-json
dyn-json.cpp
${CMAKE_CURRENT_SOURCE_DIR}/json.inc
json.g
)

TARGET_LINK_YOCTO(dyn-json lang)


ENABLE_TESTING()

ADD_TEST(Module  test-lang module)
ADD_TEST(Source  test-lang source)
#ADD_TEST(Pattern test-lang pattern)
ADD_TEST(POSIX   test-lang posix)
ADD_TEST(Types   test-lang types)
