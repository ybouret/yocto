PROJECT(file2data)

ADD_EXECUTABLE(yocto_file2data file2data.cpp)
TARGET_LINK_YOCTO(yocto_file2data)

INSTALL( TARGETS yocto_file2data RUNTIME DESTINATION bin )


