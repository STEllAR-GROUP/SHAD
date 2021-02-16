# Try to find the HPX runtime system
# Input variables:
#   HPX_ROOT   - The HPX install directory
# Output variables:
#   HPX_FOUND          - System has HPX
#   HPX_INCLUDE_DIRS   - The HPX include directories
#   HPX_LIBRARIES      - The HPX libraries needed to use gperftools

include(FindPackageHandleStandardArgs)

#if (NOT DEFINED HPX_FOUND)
#  # Check that MPI is available
#  find_package(MPI REQUIRED)
#  find_package(Threads REQUIRED)
#
#  # Set default search paths
#  if (HPX_ROOT)
#    set(HPX_INCLUDE_DIR ${HPX_ROOT}/include CACHE PATH "The include directory for HPX")
#    set(HPX_LIBRARY_DIR ${HPX_ROOT}/lib CACHE PATH "The library directory for HPX")
#  endif()

  #find_path(HPX_INCLUDE_DIRS NAMES hpx/hpx.h HINTS ${HPX_INCLUDE_DIR})

  # Search for the HPX library
  find_package(HPX)
  if (HPX_FOUND)
    #set(DCA_HAVE_HPX TRUE)
    #dca_add_haves_define(DCA_HAVE_HPX)
    include_directories(${HPX_INCLUDE_DIRS})
    #list(APPEND HPX_INCLUED_DIR ${CMAKE_PTHREADS_INCLUDE_DIR})
    #list(APPEND HPX_LIBRARIES rt)
    #list(APPEND HPX_LIBRARIES ${HPX_LIBRARY})
    #list(APPEND HPX_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    #list(APPEND HPX_LIBRARIES ${MPI_LIBRARIES})
  endif()

  #find_library(HPX_LIBRARY hpx HINTS ${HPX_LIBRARY_DIR})
  #if (HPX_LIBRARY)
  #  list(APPEND HPX_INCLUED_DIR ${CMAKE_PTHREADS_INCLUDE_DIR})
  #  list(APPEND HPX_LIBRARIES rt)
  #  list(APPEND HPX_LIBRARIES ${HPX_LIBRARY})
  #  list(APPEND HPX_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
  #  list(APPEND HPX_LIBRARIES ${MPI_LIBRARIES})
  #endif()

#  find_package_handle_standard_args(HPX
#    FOUND_VAR HPX_FOUND
#    REQUIRED_VARS HPX_INCLUDE_DIRS #HPX_LIBRARIES
#    HANDLE_COMPONENTS)
#  mark_as_advanced(HPX_INCLUDE_DIR  HPX_INCLUDE_DIRS ) #HPX_LIBRARY HPX_LIBRARIES
#endif()
