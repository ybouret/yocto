SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

SET( latex_srcs "split" )

FOREACH(item IN LISTS latex_srcs)
	MESSAGE(STATUS "Cleaning temporary files for '${item}'")
	SET(to_remove "${item}.aux;${item}.synctex.gz;${item}.log;${item}.pdf")
	FOREACH(sub IN LISTS to_remove)
		IF(EXISTS "${sub}")
			MESSAGE(STATUS "removing ${sub}")
			EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove -f ${sub})
		ENDIF()
	ENDFOREACH(sub)
ENDFOREACH(item)
