# sample code
ADD_CUSTOM_COMMAND(
OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/nwsrt.inc
COMMAND yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/nwsrt.g ${CMAKE_CURRENT_SOURCE_DIR}
DEPENDS yocto-dynamo ${CMAKE_CURRENT_SOURCE_DIR}/nwsrt.g
)

ADD_EXECUTABLE(nwsrt
nwsrt.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nwsrt.inc
nwsrt.g)
TARGET_LINK_YOCTO(nwsrt lang)

