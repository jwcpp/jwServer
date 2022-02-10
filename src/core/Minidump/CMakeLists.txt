############################################################
# Minidump library
############################################################

start_project(LIBRARY)

add_source(dumper.h)

if(WIN32)
	add_source(mdump.cpp)
else()
	add_source(gdbdmp.cpp)
endif()

depend_on_project(Common)
depend_on_project(Platform)

end_project()
