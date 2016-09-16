########################################################################
##
##
## Detecting OpenCL and setting variables
##                    -- set    YOCTO_OCL_FOUND
##                    -- define YOCTO_OCL_LINK_TO
##
########################################################################
SET(YOCTO_OCL_FOUND OFF)
MESSAGE( STATUS "" )
MESSAGE( STATUS "[OpenCL] Detecting Installation..." )

########################################################################
##
## for Apple...
##
########################################################################
IF(YOCTO_DARWIN)
	MESSAGE( STATUS "[OpenCL] Using Apple OpenCL" )
	SET(YOCTO_OCL_FOUND ON)
	
	#adjust linking macro
	MACRO(YOCTO_OCL_LINK_TO tgt)
		MESSAGE( STATUS "${tgt} will use OpenCL (Apple)" )
		TARGET_LINK_LIBRARIES( ${tgt} "-framework OpenCL")
	ENDMACRO(YOCTO_OCL_LINK_TO)
ENDIF()

########################################################################
##
## for AMD implementation
##
########################################################################
IF(NOT YOCTO_OCL_FOUND)
	SET( AMDAPPSDKROOT $ENV{AMDAPPSDKROOT} )
	MESSAGE( STATUS "[OpenCL] Looking for AMD" )
	IF( NOT "${AMDAPPSDKROOT}" STREQUAL "" )
		MESSAGE( STATUS "[OpenCL] Using AMD implementation" )
		SET(YOCTO_OCL_FOUND ON)
		#---------------------------------------------------------------
		# register the include path
		#---------------------------------------------------------------
		INCLUDE_DIRECTORIES( "${AMDAPPSDKROOT}/include" )
		IF( YOCTO64 )
			MESSAGE( STATUS "[OpenCL] 64 bits" )
			LINK_DIRECTORIES( "${AMDAPPSDKROOT}/lib/x86_64" )
		ELSE()
			MESSAGE( STATUS "[OpenCL] 32 bits" )
			LINK_DIRECTORIES( "${AMDAPPSDKROOT}/lib/x86" )
		ENDIF()
		
		#---------------------------------------------------------------
		# Use OpenCL 
		#---------------------------------------------------------------
		MACRO( YOCTO_OCL_LINK_TO tgt )
			MESSAGE( STATUS "${tgt} will use OpenCL (AMD)" )
			TARGET_LINK_LIBRARIES( ${tgt} "OpenCL")
		ENDMACRO(YOCTO_OCL_LINK_TO)
	ENDIF()
	
ENDIF()


########################################################################
##
## for Linux: looking for system OpenCL if not so far
##
########################################################################
IF(YOCTO_LINUX AND NOT YOCTO_OCL_FOUND)
	MESSAGE( STATUS "[OpenCL] Looking for system OpenCL" )
	FIND_FILE(CL-H cl.h PATHS /usr/include/CL /usr/local/cuda/include/CL)
	MESSAGE( STATUS "[OpenCL] cl.h=${CL-H}" )
	
	IF( NOT "${CL-H}" STREQUAL "CL-H-NOTFOUND"  )
		MESSAGE( STATUS "[OpenCL] found system cl.h"  )
		SET(YOCTO_OCL_FOUND ON)
	
		#adjust include
		GET_FILENAME_COMPONENT(OPENCL_INCLUDE_CL ${CL-H} PATH )
		GET_FILENAME_COMPONENT(OPENCL_INCLUDE    ${OPENCL_INCLUDE_CL} PATH )
		MESSAGE( STATUS "[OpenCL] in ${OPENCL_INCLUDE}" )	
		INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE} )
		
		#adjust linking macro
		MACRO(YOCTO_OCL_LINK_TO tgt)
			MESSAGE( STATUS "${tgt} will use OpenCL (Linux/System)" )
			TARGET_LINK_LIBRARIES( ${tgt} "OpenCL")
		ENDMACRO(YOCTO_OCL_LINK_TO)
	ENDIF()
	
ENDIF()

########################################################################
##
## try Windows cuda
##
########################################################################
IF(YOCTO_WINDOWS AND NOT YOCTO_OCL_FOUND)
	MESSAGE( STATUS "[OpenCL] Looking for CUDA" )
	SET(OPENCL_INCLUDE_PATH "$ENV{CUDA_INC_PATH}" )
	SET(OPENCL_LIBRARY_PATH "$ENV{CUDA_LIB_PATH}" )
	
	IF( NOT "${OPENCL_INCLUDE_PATH}" STREQUAL "" )
		MESSAGE(STATUS "[OpenCL] using CUDA/Windows")
		SET(YOCTO_OCL_FOUND ON)
		INCLUDE_DIRECTORIES( "${OPENCL_INCLUDE_PATH}" )
		LINK_DIRECTORIES(    "${OPENCL_LIBRARY_PATH}/../Win32" )
		MACRO(YOCTO_OCL_LINK_TO tgt )
			MESSAGE( STATUS "${tgt} will use OpenCL (CUDA/Windows)" )
			TARGET_LINK_LIBRARIES( ${tgt} "OpenCL")
		ENDMACRO(YOCTO_OCL_LINK_TO)
	ENDIF()
	
ENDIF()



########################################################################
##
##
## End Detecting OpenCL
##
##
########################################################################

IF(YOCTO_OCL_FOUND)
	MESSAGE( STATUS "[OpenCL] Enabled" )
ELSE()
	MESSAGE( STATUS "[OpenCL] Disabled" )
ENDIF()
MESSAGE( STATUS "" )

