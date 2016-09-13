########################################################################
##
##
## global parameters
##
##
########################################################################
MESSAGE("===> <yocto>")
SET(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES ON)
SET(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES     ON)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS    ON)

########################################################################
##
##
## Find out Platform for specific flags
##
##
########################################################################
MESSAGE("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

STRING(COMPARE EQUAL "Darwin"  ${CMAKE_SYSTEM_NAME} YOCTO_DARWIN)
STRING(COMPARE EQUAL "FreeBSD" ${CMAKE_SYSTEM_NAME} YOCTO_FREEBSD)
STRING(COMPARE EQUAL "Linux"   ${CMAKE_SYSTEM_NAME} YOCTO_LINUX)
STRING(COMPARE EQUAL "SunOS"   ${CMAKE_SYSTEM_NAME} YOCTO_SUNOS)
STRING(COMPARE EQUAL "OpenBSD" ${CMAKE_SYSTEM_NAME} YOCTO_OPENBSD)



########################################################################
##
## end of yocto.cmake
##
########################################################################
MESSAGE("===> </yocto>")
