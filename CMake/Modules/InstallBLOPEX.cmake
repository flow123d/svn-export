#InstallBoost.cmake
#
# Created on: Jul 20, 2012
#     Author: jb
#

if (NOT EXTERNAL_BLOPEX_DIR)
    set(EXTERNAL_BLOPEX_DIR "${EXTERNAL_PROJECT_DIR}/blopex_build")
endif()    

##########################################################################
# Download BLOPEX
# A temporary CMakeLists.txt
set (cmakelists_fname "${EXTERNAL_BLOPEX_DIR}/CMakeLists.txt")
file (WRITE "${cmakelists_fname}"
"
  ## This file was autogenerated by InstallBLOPEX.cmake
  cmake_minimum_required(VERSION 2.8)
  include(ExternalProject)
  ExternalProject_Add(BLOPEX
    DOWNLOAD_DIR ${EXTERNAL_BLOPEX_DIR} 
    URL \"http://bacula.nti.tul.cz/~jan.brezina/flow123d_libraries/blopex.tar.gz\"
    #URL \"/home/jb/local/blopex.tar.gz\"
    SOURCE_DIR ${EXTERNAL_BLOPEX_DIR}/src
  )
")

message(STATUS "=== Installing BLOPEX ===")
# run cmake
execute_process(COMMAND ${CMAKE_COMMAND} ${EXTERNAL_BLOPEX_DIR} 
    WORKING_DIRECTORY ${EXTERNAL_BLOPEX_DIR})

find_program (MAKE_EXECUTABLE NAMES make gmake)
# run make
execute_process(COMMAND ${MAKE_EXECUTABLE} BLOPEX
   WORKING_DIRECTORY ${EXTERNAL_BLOPEX_DIR})    


#file (REMOVE ${cmakelists_fname})

message(STATUS "== BLOPEX build done")