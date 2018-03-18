
FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-visit main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-visit visit)
TARGET_LINK_LIBRARIES(test-visit simV2 ${MPI_LIBRARIES})

ADD_CUSTOM_TARGET(install_loop_ui ALL
${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/loop.ui $ENV{HOME}/.visit/ui/loop.ui
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/loop.ui)
