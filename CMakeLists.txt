# CMAKE system TODO
#
# * make our own module FindPETSC  using native cmake support in PETSC version >3.2 
# * check external packages in FindPETSC, in particular Parmetis
#
# * report compiler flags and used libraries in a summarizing table before own build
#
# * use CTest to perform unit tests
# 
# * use CPack to  make Windows and Linux packages
#
#


# Include settings given by user first
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(${CMAKE_CURRENT_SOURCE_DIR}/makefile.in.cmake )


# If the CCACHE_CXX_COMPILER is set 
# then the link to this compiler with ccache is created and used.
if(CCACHE_CXX_COMPILER)
    set (CMAKE_CXX_COMPILER "ccache_c++")
    configure_file(${CMAKE_SOURCE_DIR}/CMake/ccache_c++_link_template ${CMAKE_BINARY_DIR}/ccache_c++)
endif(CCACHE_CXX_COMPILER)


# Project name and language(s)
project(Flow123d CXX C Fortran)

# CMake version required for pre-build this project
cmake_minimum_required(VERSION 2.8)
if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  cmake_policy(VERSION 2.8.4)
  cmake_policy(SET CMP0017 NEW)
endif()

################################################################################# set CMake Modules
# Set all module paths
set(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
list(APPEND CMAKE_MODULE_PATH "${Flow123d_SOURCE_DIR}/CMake/Modules")

if (NOT EXTERNAL_PROJECT_DIR)
    # default directory for building external libraries 
    set(EXTERNAL_PROJECT_DIR ${PROJECT_SOURCE_DIR}/external_projects)
endif()


############################################################################################# set compiler
# check C++11 properties of the compiler
# Initialize CXXFLAGS.




##############
# set appropriate compiler flags for debug/release compilation modes
message(STATUS "Flow build type: ${FLOW_BUILD_TYPE} C++ compiler falgs: ${FLOW_CC_FLAGS}")




if(FLOW_CC_FLAGS) 
  # user given flags overrides everything  
  message(STATUS "Using user specified compiler options: ${FLOW_CC_FLAGS}")
  set(CMAKE_CXX_FLAGS  "${FLOW_CC_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${FLOW_CC_FLAGS}")
  
else(FLOW_CC_FLAGS)
 
  message(STATUS "Using build type: ${FLOW_BUILD_TYPE}")
  if(FLOW_BUILD_TYPE STREQUAL "debug")
    # debug mode
    set(CMAKE_CXX_FLAGS "-g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "-g ")
    add_definitions(-DDEBUG)
  elseif(FLOW_BUILD_TYPE STREQUAL "release")
    # Release mode
    set(CMAKE_CXX_FLAGS "-O3")
    add_definitions(-DNODEBUG)
  elseif(FLOW_BUILD_TYPE STREQUAL "release_with_debug")
    # Release mode with debug info 
    set(CMAKE_CXX_FLAGS "-O3")
    add_definitions(-DDEBUG)
  else()
  endif()
  
endif(FLOW_CC_FLAGS)


# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")        
	    execute_process(
	        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	    if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
	        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	        add_definitions("-DHAVE_CXX11_FULL")    
	        set(CMAKE_HAVE_CXX11 "true")
	    elseif (GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
	        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	        add_definitions("-DHAVE_CXX11_DRAFT")
	        set(CMAKE_HAVE_CXX11 "true")   
	    elseif (GCC_VERSION VERSION_GREATER 4.4)
	    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	    	set(CMAKE_HAVE_CXX11 "true")   
	    	message(WARNING "Warnning: GCC version under 4.6, not recomended!." ) 
	    else ()
	    	message(SEND_ERROR "Error: GCC version under 4.4, not supported!." )	
	    endif ()

    # Necessary for FParser - TODO: move to better place
    # ?? what about Clang
    add_definitions("-DHAVE_SINCOS")
    
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++") # for some reason BOOST doesn't work with option -std=c++11
    
    # BOOST must be compiled with clang in order to use C++11 standard. 
    set(Boost_FORCE_REBUILD "true")
    set(Boost_CONFIG_FLAGS " --with-toolset=clang ")
    set(Boost_BUILD_FLAGS " toolset=clang cxxflags=\"-stdlib=libc++\" linkflags=\"-stdlib=libc++\" " )        
    
    #add_definitions("-DHAVE_CXX11")
    set(CMAKE_HAVE_CXX11 "true")
    # 
    # TODO: make test for presence of sincos() function and set HAVE_SINCOS appropriatly, used in fparser extsrc/fpaux.cc
    #
else ()
    message(STATUS "Your C++ compiler does not support C++11. Some features may be disabled.")
endif()

#########################################################################
# Check that glibc has working execinfo.h
include(CheckIncludeFile)
check_include_file("execinfo.h" EXEC_INFO_H)
if (EXEC_INFO_H) 
    add_definitions("-DHAVE_EXEC_INFO")
    
    MESSAGE(STATUS "Looking for abi::__cxa_demangle")
    TRY_COMPILE(HAVE_DEMAGLER
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/CMake/CheckDemagler.cpp
    )
    
    IF(HAVE_DEMAGLER)
      MESSAGE(STATUS "Looking for abi::__cxa_demangle - found")
      add_definitions("-DHAVE_DEMAGLER")
    ELSE(HAVE_DEMAGLER)
      MESSAGE(STATUS "Looking for abi::__cxa_demangle - not found")
    ENDIF(HAVE_DEMAGLER)
    
endif(EXEC_INFO_H)




######################################################################### find libraries
#
# find boost or use our own
#
# - Try to find Boost include dirs and libraries
#
# NOTE: Take note of the Boost_ADDITIONAL_VERSIONS variable below.
# Due to Boost naming conventions and limitations in CMake this find
# module is NOT future safe with respect to Boost version numbers,
# and may break.
#
# Input variables:
#
#   Boost_USE_MULTITHREADED      Can be set to OFF to use the non-multithreaded
#   Boost_USE_STATIC_LIBS        Can be set to ON to force the use of the static
#   Boost_NO_SYSTEM_PATHS        Set to TRUE to suppress searching in system (or other locations outside of BOOST_ROOT or BOOST_INCLUDEDIR).
#   Boost_USE_STATIC_RUNTIME     If enabled, searches for boost libraries linked against a static C++ standard library
#   Boost_USE_DEBUG_PYTHON       
#   Boost_USE_STLPORT            If enabled, searches for boost libraries compiled against the STLPort standard
#   Boost_USE_STLPORT_DEPRECATED_NATIVE_IOSTREAMS    If enabled, searches for boost libraries compiled against the deprecated STLPort "native iostreams" feature ('n' ABI tag).
#   Boost_ADDITIONAL_VERSIONS    A list of version numbers to use for searching
#   Boost_DEBUG                  Set this to TRUE to enable debugging output
#   Boost_DETAILED_FAILURE_MSG   FindBoost doesn't output detailed information
#   Boost_COMPILER               Set this to the compiler suffix used by Boost
#   Boost_THREADAPI                When building boost.thread, sometimes the name of the library contains an additional "pthread" or "win32"
#   Boost_REALPATH               Resolves symbolic links for discovered boost libraries
#
#
#   BOOST_ROOT or BOOSTROOT      The preferred installation prefix for searching for
#                                back on the system paths, set Boost_NO_SYSTEM_PATHS
#   BOOST_INCLUDEDIR             Set this to the include directory of Boost, if the
#   BOOST_LIBRARYDIR             Set this to the lib directory of Boost, if the
#
# Variables defined by this module:
#
#   Boost_FOUND                         System has Boost, this means the include dir was
#   Boost_INCLUDE_DIRS                  Boost include directories: not cached
#   Boost_INCLUDE_DIR                   This is almost the same as above, but this one is cached and may be modified by advanced users
#   Boost_LIBRARIES                     Link to these to use the Boost libraries that you
#   Boost_LIBRARY_DIRS                  The path to where the Boost library files are.
#   Boost_VERSION                       The version number of the boost libraries that
#   Boost_LIB_VERSION                   The version number in filename form as
#   Boost_MAJOR_VERSION                 major version number of boost
#   Boost_MINOR_VERSION                 minor version number of boost
#   Boost_SUBMINOR_VERSION              subminor version number of boost
#   Boost_LIB_DIAGNOSTIC_DEFINITIONS    [WIN32 Only] You can call
#   Boost_${COMPONENT}_FOUND            True IF the Boost library "component" was found.
#   Boost_${COMPONENT}_LIBRARY          Contains the libraries for the specified Boost

set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     OFF)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(EXTERNAL_BOOST_DIR  "${EXTERNAL_PROJECT_DIR}/boost_build")  
set(REQUIRED_Boost_COMPONENTS program_options serialization regex)
 
#set(Boost_DEBUG TRUE)
# For some mysterious reason, the second find_package fails to find
# components of boost in our own build, if we hint BOOST_ROOT of our build to the
# first find_package.
#
# The root of the error is in FindBoost, when it calls find_library for individual components.
# Success scenario:
# 1) First find_package didn't found Boost (in system direcotries)
# 2) we install our Boost
# 3) the second find_package calls find_library to find boost components and succed
#
# Fail scenario:
# 1) First find_package didn't found Boost (we set BOOST_ROOT to the empty target directory of the external project)
# 2) we install our Boost
# 3) the second find_package calls find_library to find boost components and fail
#    the parameters of the find_library are exactly same as in the previous case.
#
#set(BOOST_ROOT ${EXTERNAL_BOOST_DIR})
if (NOT Boost_FORCE_REBUILD)
    find_package(Boost 1.40.0 COMPONENTS ${REQUIRED_Boost_COMPONENTS} )
endif()    

if (NOT Boost_FOUND) 
  message(STATUS "Try to install our own Boost.")

  include(InstallBoost)  
  
  # we try to erease from the cache everything that can cause failure of the second find_package
  # however just unsetting Boost_INCLUDE_DIR should be enough
  
 
  unset(Boost_FOUND CACHE) 
  unset(Boost_INCLUDE_DIR CACHE)        # overwrite cache    
  unset(Boost_LIBRARY_DIRS CACHE)
  unset(Boost_LIB_VERSION CACHE)
  unset(Boost_VERSION CACHE)
  foreach(COMPONENT ${REQUIRED_Boost_COMPONENTS})
      string(TOUPPER ${COMPONENT} COMPONENT)

      unset(Boost_${COMPONENT}_FOUND CACHE)
      unset(Boost_${COMPONENT}_LIBRARY CACHE)
      unset(Boost_${COMPONENT}_LIBRARY_DEBUG CACHE)
      unset(Boost_${COMPONENT}_LIBRARY_RELEASE CACHE)
            
  endforeach(COMPONENT)
  
  set(BOOST_ROOT ${EXTERNAL_BOOST_DIR})    
  find_package( Boost 1.42.0 REQUIRED COMPONENTS ${REQUIRED_Boost_COMPONENTS}  )
   
endif()

message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")   
message(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")


# We are looking for PETSC, f succed this module set:
#  PETSC_FOUND        - system has PETSc
#  PETSC_INCLUDES     - the PETSc include directories
#  PETSC_LIBRARIES    - Link these to use PETSc
#  PETSC_COMPILER     - Compiler used by PETSc, helpful to find a compatible MPI
#  PETSC_DEFINITIONS  - Compiler switches for using PETSc
#  PETSC_MPIEXEC      - Executable for running MPI programs
#  PETSC_VERSION      - Version string (MAJOR.MINOR.SUBMINOR)
include (FindPackageMultipass)
  
find_package(PETSc 3.4.0)


if (NOT PETSC_FOUND)
    if (PETSC_DIR) 
        message(STATUS "Can not configure with given PETSC_DIR=${PETSC_DIR}")
    else()    
        # Install PETSC
        # also set PETSC_DIR, PETSC_ARCH
        set(EXTERNAL_PETSC_DIR  ${EXTERNAL_PROJECT_DIR}/petsc_build)
        include(InstallPetsc)
        
        set(PETSC_CURRENT "NO")
        find_package(PETSc 3.4.0)
        
    endif()    
        
        
    # If we haven't PETSC, we try to find MPI
        
    # === Variables ===
    # This module will set the following variables per language in your project,
    # where <lang> is one of C, CXX, or Fortran:
    #   MPI_<lang>_FOUND           TRUE if FindMPI found MPI flags for <lang>
    #   MPI_<lang>_COMPILER        MPI Compiler wrapper for <lang>
    #   MPI_<lang>_COMPILE_FLAGS   Compilation flags for MPI programs
    #   MPI_<lang>_INCLUDE_PATH    Include path(s) for MPI header
    #   MPI_<lang>_LINK_FLAGS      Linking flags for MPI programs
    #   MPI_<lang>_LIBRARIES       All libraries to link MPI programs against
    # Additionally, FindMPI sets the following variables for running MPI
    # programs from the command line:
    #   MPIEXEC                    Executable for running MPI programs
    #   MPIEXEC_NUMPROC_FLAG       Flag to pass to MPIEXEC before giving
    #                              it the number of processors to run on
    #   MPIEXEC_PREFLAGS           Flags to pass to MPIEXEC directly
    #                              before the executable to run.
    #   MPIEXEC_POSTFLAGS          Flags to pass to MPIEXEC after other flags
#
#    find_package(MPI)
#  
#    if (MPI_CXX_FOUND)
#        add_definitions(-DHAVE_MPI)
#    else(MPI_CXX_FOUND) 
        # build single processor replacement taken from PETSc
#        set(mpiuni_sources    system/mpiuni/mpi.c)
#        set(MPIUNI_INCLUDES src/system/mpiuni)
#        add_definitions(-DHAVE_MPI -DHAVE_MPIUNI)
#    
        # overwrite NOTFOUND status
#        set(MPI_CXX_LIBRARIES mpiuni_lib)
#        set(MPI_CXX_INCLUDE_PATH ${MPIUNI_INCLUDES})
#    
#    endif(MPI_CXX_FOUND)

endif(NOT PETSC_FOUND)

if(PETSC_FOUND)
    add_definitions(-DHAVE_PETSC -DHAVE_MPI)
    set(MPI_CXX_LIBRARIES ${PETSC_EXTERNAL_LIB})
    set(MPI_CXX_INCLUDE_PATH ${PETSC_INCLUDES})
    
    message(STATUS "PETSC_LIBS: ${PETSC_LIBRARIES}")
    message(STATUS "PETSC_EXTERNAL_LIB: ${PETSC_EXTERNAL_LIB}")
    message(STATUS "PETSC_EXTERNAL_LIB: ${petsc_libraries_external}")
endif(PETSC_FOUND)

find_package(PARMETIS)
#################################################################################
#  BDDCML_FOUND - set to true if the library is found
#set(EXTERNAL_BLOPEX_DIR  ${EXTERNAL_PROJECT_DIR}/blopex_build)
#include(InstallBLOPEX)
#set(EXTERNAL_BDDCML_DIR  ${EXTERNAL_PROJECT_DIR}/bddcml_build)
#include(InstallBDDCML)

#################################################################################
#  Armadillo_FOUND - set to true if the library is found
#  Armadillo_INCLUDE_DIRS - list of required include directories
#  Armadillo_LIBRARIES - list of libraries to be linked 
#  Armadillo_CONFIGURE_CMAKE - path to configure cmake script that introduce target 'armadillo'
#                              with correctly set dependencies on other libraries

find_package(ArmadilloFlow 3.2.4)
if (NOT Armadillo_FOUND)
  message(STATUS "Try to install our own Armadillo.")
  set(EXTERNAL_ARMADILLO_DIR  ${EXTERNAL_PROJECT_DIR}/armadillo_build)
  include(InstallArmadillo)

  # we try to erease from the cache everything that can cause failure of the second find_package
  # however just unsetting Boost_INCLUDE_DIR should be enough
  unset(Armadillo_FOUND CACHE) 
  unset(Armadillo_INCLUDE_DIR CACHE)        # overwrite cache    
  unset(Armadillo_LIBRARY_DIRS CACHE)
  unset(Armadillo_CONFIGURE_FILE CACHE)

  set(Armadillo_ROOT_HINT ${EXTERNAL_ARMADILLO_DIR})  
  find_package(ArmadilloFlow 3.2.4 REQUIRED)

endif()


message(STATUS "Armadillo_LIBRARIES: ${Armadillo_LIBRARIES}")
message(STATUS "Armadillo_INCLUDE_DIRS: ${Armadillo_INCLUDE_DIRS}")
message(STATUS "Armadillo_CONFIGURE_FILE: ${Armadillo_CONFIGURE_FILE}")
include("${Armadillo_CONFIGURE_FILE}")   # this introduce target "armadillo" with correct dependencies
IF (PETSC_FOUND)
    # until we know how to take only blas and lapack from PETSC we have to make Armadillo depend on whole PETSC lib.
    # this may be fragile because of _NOCONFIG
    set_target_properties(armadillo PROPERTIES 
      IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "${PETSC_EXTERNAL_LIB}"
      ) 
endif()

# optionaly we use Python interface
# We disallow python under cygwin since we are not able to extract correct libraries to make it work elsewhere.
if (CYGWIN)
else (CYGWIN) 
    find_package(PythonLibs)
    if (PYTHONLIBS_FOUND)
        include_directories( ${PYTHON_INCLUDE_DIRS})
        add_definitions(-DHAVE_PYTHON)
    else()
	set(PYTHON_INCLUDE_DIRS "")
	set(PYTHON_LIBRARIES "")
    endif()
endif(CYGWIN)

# check for subversion and prepare generating of file with revision info.
find_package(Subversion)



################################################################### set includes and libraries paths

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}                            # include the output directory, where the svnversion.h file is generated    
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/fparser-4.5.1
    ${PETSC_INCLUDES}
    ${Boost_INCLUDE_DIR}
    ${Armadillo_INCLUDE_DIRS}
    ${MPI_CXX_INCLUDE_PATH}
)


##############

############################################################33 process subdirectories


# add fparser sources
message(STATUS "ad subdir FParser")
add_subdirectory(third_party/fparser-4.5.1)

# add various our sources
add_subdirectory(src)
add_subdirectory(doc)

add_subdirectory(bin/tests)
add_subdirectory(test_units)

#################################################################### add targets


if( PETSC_FOUND )

  #### create rev_num.h - file with revision number
  # a custom target - is always built -> call cmake script to generate rev_num.h
  add_custom_target(rev_num_always 
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/Flow123d_svn_rev_num.cmake)
  
  # rev_num.h is a generated file
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/rev_num.h
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)


  #### make a script which call correct mpiexec (taken from PETSC)
  # we can not use link, since mpiexec can be sript which reads its location from calling sequence
  #
  # TODO: otestovat if(EXISTS ...) pod Windows, na tomto miste se mu nedari nalezt mpiexec ktery existuje.
  # ? Potrebuje pod Cygwin uplnou cestu C:\cygwin\ ...? 
  message(STATUS "petsc_mpiexec: ${PETSC_MPIEXEC}")
  if(PETSC_MPIEXEC)     # missing CMAKE command to test working executable, moreover it could be without full path
      message(STATUS "using petsc_mpiexec: ${PETSC_MPIEXEC}")
      set(MPIEXEC_PATH ${PETSC_MPIEXEC})
  else()
      if(COMMAND mpiexec)
        message(WARNING "Missing mpiexec in PETSc instalation. Using system wide mpiexec.")
        set(MPIEXEC_PATH mpiexec)
      else()
        message(WARNING "Missing any mpiexec.")
      endif()
  endif()
  configure_file(${CMAKE_SOURCE_DIR}/CMake/mpiexec_link_template ${CMAKE_BINARY_DIR}/mpiexec)
  


  ### add target for flow123d binary
  add_executable(flow123d src/main.cc)
  target_link_libraries(flow123d  flow123d_lib  ${PETSC_ADDITIONAL_LIBS}  semchem  ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})     # set libraries linked to main binary (order matter)
  add_dependencies(flow123d rev_num_always mpiexec_link)      # make mpiexec_link when flow123d executable is built

  # explicitly say that source file depends on rev_num_always in order to have actual revision number
  set_source_files_properties(src/main.cc OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/rev_num.h )
else()
  message(STATUS "PETSC not found. Omitting configuration of the target 'flow123d'.")
endif()


