####
# this CMake script is called at build time to get revision of current working copy
#
# Input variables:
# WC_DIRECTORY - working copy directory to check for revision
# WC_REVISION - set revision manually

# the FindSubversion.cmake module is part of the standard distribution
include(FindSubversion)

message(STATUS "dir: ${WC_DIRECTORY}")
message(STATUS "rev: ${WC_REVISION}")
if (DEFINED WC_REVISION)
	set(Flow_WC_REVISION "${WC_REVISION}")
	set(Flow_WC_URL "(unknown)")
else()
	if (Subversion_FOUND)
		# extract working copy information for SOURCE_DIR into Flow variable
		Subversion_WC_INFO(${WC_DIRECTORY} Flow)
	else ()
		message(SEND_ERROR "Subversion client not found. Needed to detect revision.")
	endif()

	if(Flow_WC_REVISION)
		message(STATUS "SVN working copy revision: ${Flow_WC_REVISION}")
		message(STATUS "SVN working copy URL: ${Flow_WC_URL}")	
	else()
		message(STATUS "SVN ERROR: Can not detect revision. Please, fix the problem specified above or set Flow_WC_REVISION manually.")
	endif()
endif()

# write a file with the SVN VERSION and URL define
file(WRITE rev_num.h.tmp "#define _PROGRAM_REVISION_ \"${Flow_WC_REVISION}\"\n#define _PROGRAM_BRANCH_ \"${Flow_WC_URL}\"\n")

# copy the file to the final header only if the version changes
# reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different rev_num.h.tmp rev_num.h)
#execute_process(COMMAND ${CMAKE_COMMAND} -E remove rev_num.h.tmp)