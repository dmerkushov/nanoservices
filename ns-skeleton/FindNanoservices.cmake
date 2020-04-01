# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindNanoservices
# -------
#
# Finds the ns-skeleton library
#

function(str_right str len output)
	string(LENGTH ${str} STR_LEN)
	math(EXPR STR_LEN ${STR_LEN}-${len})
	string(SUBSTRING ${str} 0 ${STR_LEN} str)
	set(${output} ${str} PARENT_SCOPE)
endfunction(str_right)


execute_process(COMMAND dpkg-query -W -f=\${Package}\\tProvides:\${Provides}\\n
		COMMAND grep -E "nanoservices$"
		COMMAND sed -e "s/\\\(.*\\\)\\t.*/\\1/"
	OUTPUT_VARIABLE SKELETON_LIB_DEB)
execute_process(COMMAND dpkg-query -W -f=\${Package}\\tProvides:\${Provides}\\n
		COMMAND grep -E "nanoservices-dev$"
		COMMAND sed -e "s/\\\(.*\\\)\\t.*/\\1/"
	OUTPUT_VARIABLE SKELETON_DEV_DEB)

str_right(${SKELETON_LIB_DEB} 1 SKELETON_LIB_DEB)

execute_process(COMMAND dpkg -L ${SKELETON_LIB_DEB}
		COMMAND grep ".so"
		COMMAND grep lib
		OUTPUT_VARIABLE LIBNSSKELETON_LIBRARY)

str_right(${LIBNSSKELETON_LIBRARY} 1 LIBNSSKELETON_LIBRARY)

get_filename_component(LIBNSSKELETON_LIBRARY_DIR ${LIBNSSKELETON_LIBRARY} PATH)

str_right(${SKELETON_DEV_DEB} 1 SKELETON_DEV_DEB)

execute_process(COMMAND dpkg -L ${SKELETON_DEV_DEB}
		COMMAND grep "[.]h"
		COMMAND sed -e "s/\\(.*\\)\\/.*[.]h/\\1/"
		COMMAND sort -u
		COMMAND head -1
		OUTPUT_VARIABLE LIBNSSKELETON_INCLUDE_DIR)

str_right(${LIBNSSKELETON_INCLUDE_DIR} 1 LIBNSSKELETON_INCLUDE_DIR)

# Did we find anything?
find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(Nanoservices
                                  REQUIRED_VARS LIBNSSKELETON_LIBRARY LIBNSSKELETON_INCLUDE_DIR)
set(LIBNSSKELETON_FOUND  ${NANOSERVICES_FOUND})


mark_as_advanced(LIBNSSKELETON_INCLUDE_DIR LIBNSSKELETON_LIBRARY )
if(LIBNSSKELETON_FOUND)
	set(LIBNSSKELETON_LIBRARIES ${LIBNSSKELETON_LIBRARY} )
	set(LIBNSSKELETON_LIBRARY_DIRS ${LIBNSSKELETON_LIBRARY_DIR} )
	set(LIBNSSKELETON_INCLUDE_DIRS ${LIBNSSKELETON_INCLUDE_DIR} )
endif()
