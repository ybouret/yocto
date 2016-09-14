INCLUDE_DIRECTORIES(.. ../../main)

ADD_EXECUTABLE(yocto_gram2defs gram2defs.cpp)
TARGET_LINK_YOCTO(yocto_gram2defs lingua)

INSTALL( TARGETS yocto_gram2defs RUNTIME DESTINATION bin )
