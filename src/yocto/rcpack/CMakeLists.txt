ADD_EXECUTABLE(yocto_rcpack rcpack.cpp)
TARGET_LINK_YOCTO(yocto_rcpack)

ADD_EXECUTABLE(rcdemo main.cpp)
TARGET_LINK_YOCTO(rcdemo)

ADD_DEPENDENCIES(rcdemo yocto_rcpack)

SET(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
#MESSAGE("resources@${RESOURCES}" )

ADD_CUSTOM_COMMAND(
	TARGET rcdemo 
	POST_BUILD
	COMMAND yocto_rcpack $<TARGET_FILE:rcdemo> ${RESOURCES}
	COMMENT "Adding resources to rcdemo..."  )
	
INSTALL( TARGETS yocto_rcpack RUNTIME DESTINATION bin )
