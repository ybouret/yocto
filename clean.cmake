FILE(GLOB to_remove *.dat)
MESSAGE("to_remove: ${to_remove}")
FOREACH(item IN LISTS to_remove)
	MESSAGE("will remove ${item}")
ENDFOREACH(item)


