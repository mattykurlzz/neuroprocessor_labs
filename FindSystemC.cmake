message(STATUS "Searching for SystemC")

set(_SYSTEMC_HINTS
  $ENV{HOME}/lib/systemc/include
  $ENV{HOME}/lib/systemc/lib
  $ENV{HOME}/lib/systemc/lib-linux  
  $ENV{HOME}/lib/systemc/lib-linux64
  $ENV{HOME}/lib/systemc-2.3.3/include
  $ENV{HOME}/lib/systemc-2.3.3/lib
  $ENV{HOME}/lib/systemc-2.3.3/lib-linux  
  $ENV{HOME}/lib/systemc-2.3.3/lib-linux64
  )
set(_SYSTEMC_PATHS
  /usr/include
  /usr/lib
  /usr/lib-linux
  /usr/lib-linux64
  /usr/local/include
  /usr/local/lib
  /usr/local/lib-linux  
  /usr/local/lib-linux64
  /opt/systemc/include
  /opt/systemc/lib
  /opt/systemc/lib-linux  
  /opt/systemc/lib-linux64
  )
find_file(_SYSTEMC_VERSION_FILE
  NAMES sc_ver.h
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
  PATH_SUFFIXES sysc/kernel
)

if(NOT _SYSTEMC_VERSION_FILE)
  message(STATUS "SystemC library NOT FOUND")
else()
  # SystemC < 2.3.1
  exec_program("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 7 "
              OUTPUT_VARIABLE SystemC_MAJOR)
  exec_program("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 8 "
              OUTPUT_VARIABLE SystemC_MINOR)
  exec_program("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 9 "
              OUTPUT_VARIABLE SystemC_REV)

  # SystemC >= 2.3.1
  if("${SystemC_MAJOR}" MATCHES "")
  exec_program("grep '#define SC_VERSION_MAJOR' ${_SYSTEMC_VERSION_FILE} | cut -d ' ' -f 8"
              OUTPUT_VARIABLE SystemC_MAJOR)
  exec_program("grep '#define SC_VERSION_MINOR' ${_SYSTEMC_VERSION_FILE} | cut -d ' ' -f 8"
              OUTPUT_VARIABLE SystemC_MINOR)
  exec_program("grep '#define SC_VERSION_PATCH' ${_SYSTEMC_VERSION_FILE} | cut -d ' ' -f 8"
              OUTPUT_VARIABLE SystemC_REV)
  endif("${SystemC_MAJOR}" MATCHES "")

  set(SystemC_VERSION ${SystemC_MAJOR}.${SystemC_MINOR}.${SystemC_REV})

  if("${SystemC_MAJOR}" MATCHES "2")
    set(SystemC_FOUND TRUE)
  endif("${SystemC_MAJOR}" MATCHES "2")

  message(STATUS "SystemC version = ${SystemC_VERSION}")

  find_path(SystemC_INCLUDE_DIRS
    NAMES systemc.h
    HINTS ${_SYSTEMC_HINTS}
    PATHS ${_SYSTEMC_PATHS}
  )

  find_library(SystemC_LIBRARIES
    NAMES systemc
    HINTS ${_SYSTEMC_HINTS}
    PATHS ${_SYSTEMC_PATHS}
  ) 

  find_path(SystemC_LIBRARY_DIRS
    NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}systemc${CMAKE_SHARED_LIBRARY_SUFFIX}
    HINTS ${_SYSTEMC_HINTS}
    PATHS ${_SYSTEMC_PATHS}
  )

  message(STATUS "SystemC library = ${SystemC_LIBRARIES}")
endif()
