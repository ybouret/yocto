########################################################################
##
##
## Top level CMakeLists.txt for all projects
##
##
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
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
	MESSAGE( STATUS "Append ${dir}")
	ADD_SUBDIRECTORY(${dir})
ENDMACRO(YOCTO_BUILD)

ADD_SUBDIRECTORY(yocto)
INSTALL( FILES 
	../share/yocto.cmake
	../share/fltk.cmake
	../share/visit.cmake
	../share/R.cmake
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
YOCTO_BUILD(lang)
YOCTO_BUILD(json)
YOCTO_BUILD(seem)
YOCTO_BUILD(mk)
YOCTO_BUILD(mpi)
YOCTO_BUILD(net)
YOCTO_BUILD(z++)
YOCTO_BUILD(bz2++)
YOCTO_BUILD(gems)
YOCTO_BUILD(lua++)
YOCTO_BUILD(fltk)
YOCTO_BUILD(ink)
YOCTO_BUILD(alchemy)
YOCTO_BUILD(chemical)
YOCTO_BUILD(python)
YOCTO_BUILD(R++)
YOCTO_BUILD(ocl)
YOCTO_BUILD(visit)

