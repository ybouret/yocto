########################################################################
##
##
## Top level CMakeLists.txt for all projects
##
##
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(yocto)

########################################################################
##
## Main rules and helpers
##
########################################################################
INCLUDE(../share/yocto.cmake)

SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../sdk)

SET(YOCTO_BUILD_TESTS ON)

########################################################################
## mandatory project and files
########################################################################
MACRO(YOCTO_BUILD dir)
	MESSAGE("===> adding ${dir}")
	ADD_SUBDIRECTORY(${dir})
ENDMACRO(YOCTO_BUILD)

ADD_SUBDIRECTORY(yocto)
INSTALL( FILES 
	../share/yocto.cmake
	../build.cmake
	../purge.cmake
	../clean.cmake
	DESTINATION share/yocto )

########################################################################
##
## other projects
##
########################################################################

YOCTO_BUILD(kr)
YOCTO_BUILD(lingua)
YOCTO_BUILD(json)
YOCTO_BUILD(seem)
YOCTO_BUILD(mk)
YOCTO_BUILD(mpi)
YOCTO_BUILD(mpl)
YOCTO_BUILD(net)
