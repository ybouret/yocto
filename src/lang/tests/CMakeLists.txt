FILE(GLOB srcs test-*.cpp)

ADD_EXECUTABLE(test-lang main.cpp ${srcs}
	test.g
	simple.g
	lists.g
	json.g
	calc.g
	seem.g
    lex.g
)

TARGET_LINK_YOCTO(test-lang lang)

IF(FALSE)

	# sample code
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

ENDIF()

ENABLE_TESTING()

ADD_TEST(Module  test-lang module)
ADD_TEST(Source  test-lang source)
#ADD_TEST(Pattern test-lang pattern)
ADD_TEST(POSIX   test-lang posix)
ADD_TEST(Types   test-lang types)

 # sample code
ADD_CUSTOM_COMMAND(
OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/netsort.inc
COMMAND yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/netsort.g ${CMAKE_CURRENT_SOURCE_DIR}
DEPENDS yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/netsort.g
)

ADD_EXECUTABLE(gen-netsort
gen-netsort.cpp
${CMAKE_CURRENT_SOURCE_DIR}/netsort.inc
netsort.g
netsort.db
)

TARGET_LINK_YOCTO(gen-netsort lang)
