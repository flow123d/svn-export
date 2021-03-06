# Macro for setting up precompiled headers. Usage:
#
# add_precompiled_header(target header.h [FORCEINCLUDE])
#
# MSVC: A source file with the same name as the header must exist and
# be included in the target (E.g. header.cpp).
#
# MSVC: Add FORCEINCLUDE to automatically include the precompiled
# header file from every source file.
#
# GCC: The precompiled header is always automatically included from
# every header file.

# _targetName is the name of the target which depends on the headers
MACRO(ADD_PRECOMPILED_HEADER _targetName _input)
  # gets file name without extension NAME_WE (possible PATH|ABSOLUTE|NAME|EXT|NAME_WE)
  GET_FILENAME_COMPONENT(_inputWe ${_input} NAME_WE)
  SET(pch_source ${_inputWe}.cpp)
  FOREACH(arg ${ARGN})
    IF(arg STREQUAL FORCEINCLUDE)
      SET(FORCEINCLUDE ON)
    ELSE(arg STREQUAL FORCEINCLUDE)
      SET(FORCEINCLUDE OFF)
    ENDIF(arg STREQUAL FORCEINCLUDE)
  ENDFOREACH(arg)

# for Microsoft Visual Studio
  IF(MSVC)
    GET_TARGET_PROPERTY(sources ${_targetName} SOURCES)
    SET(_sourceFound FALSE)
    FOREACH(_source ${sources})
      SET(PCH_COMPILE_FLAGS "")
      IF(_source MATCHES \\.\(cc|cxx|cpp\)$)
GET_FILENAME_COMPONENT(_sourceWe ${_source} NAME_WE)
IF(_sourceWe STREQUAL ${_inputWe})
SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /Yc${_input}")
SET(_sourceFound TRUE)
ELSE(_sourceWe STREQUAL ${_inputWe})
SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /Yu${_input}")
IF(FORCEINCLUDE)
SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /FI${_input}")
ENDIF(FORCEINCLUDE)
ENDIF(_sourceWe STREQUAL ${_inputWe})
SET_SOURCE_FILES_PROPERTIES(${_source} PROPERTIES COMPILE_FLAGS "${PCH_COMPILE_FLAGS}")
      ENDIF(_source MATCHES \\.\(cc|cxx|cpp\)$)
    ENDFOREACH()
    IF(NOT _sourceFound)
      MESSAGE(FATAL_ERROR "A source file for ${_input} was not found. Required for MSVC builds.")
    ENDIF(NOT _sourceFound)
  ENDIF(MSVC)

# for G++
  IF(CMAKE_COMPILER_IS_GNUCXX)
    GET_FILENAME_COMPONENT(_name ${_input} NAME)
    # sets .hh files from input as _source
    SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    # sets name and makes output directory [headerfile].gch
    SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
    MAKE_DIRECTORY(${_outdir})
    # sets the name of the precompiled header
    SET(_output "${_outdir}/${CMAKE_BUILD_TYPE}.c++")
    
    STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
    SET(_compiler_FLAGS ${${_flags_var_name}})
    
    GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
    FOREACH(item ${_directory_flags})
      LIST(APPEND _compiler_FLAGS "-I${item}")
    ENDFOREACH(item)

    GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
    LIST(APPEND _compiler_FLAGS ${_directory_flags})

    SEPARATE_ARGUMENTS(_compiler_FLAGS)
    MESSAGE("command: ${CMAKE_CXX_COMPILER} -DPCHCOMPILE ${_compiler_FLAGS} -x c++-header -o {_output} ${_source}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      # the same compilation flags as for the source file are needed
      # "-DDEBUG" which is defined in makefile.in.cmake might be missing in here 
      COMMAND ${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_source}
      DEPENDS ${_source} )
    # adds the precompiled header to the target's dependencies
    ADD_CUSTOM_TARGET(${_targetName}_gch DEPENDS ${_output})
    ADD_DEPENDENCIES(${_targetName} ${_targetName}_gch)
    # inclusion of the precompiled header to the target is sets
    # and "-Winvalid-pch" makes sure that you get message if somethings worng and 
	# precompiled header is not used
    SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "-include ${_name} -Winvalid-pch")
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDMACRO(ADD_PRECOMPILED_HEADER)