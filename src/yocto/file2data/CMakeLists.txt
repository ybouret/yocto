PROJECT(file2data)

ADD_EXECUTABLE(yocto-file2data file2data.cpp)
TARGET_LINK_YOCTO(yocto-file2data)

INSTALL( TARGETS yocto-file2data RUNTIME DESTINATION bin )


