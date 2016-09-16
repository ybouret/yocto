########################################################################
##
##
## Detecting OpenCL's env:
##                    -- set    YOCTO_OCL_FOUND
##                    -- define YOCTO_OCL_LINK_TO
##
########################################################################
SET(YOCTO_OCL_FOUND OFF)
MESSAGE( STATUS "" )
MESSAGE( STATUS "[OpenCL] Detecting Installation..." )

########################################################################
##
##
##
########################################################################
IF(YOCTO_DARWIN)
	MESSAGE( STATUS "[OpenCL] Using Apple OpenCL" )
	SET(YOCTO_OCL_FOUND ON)
	
	MACRO(YOCTO_OCL_LINK_TO tgt)
		MESSAGE( STATUS "${tgt} will use OpenCL (Apple)" )
		TARGET_LINK_LIBRARIES( ${tgt} "-framework OpenCL")
	ENDMACRO(YOCTO_OCL_LINK_TO)
ENDIF()

IF(YOCTO_OCL_FOUND)
	MESSAGE( STATUS "[OpenCL] Enabled" )
ELSE()
	MESSAGE( STATUS "[OpenCL] Disable" )
ENDIF()
MESSAGE( STATUS "" )
