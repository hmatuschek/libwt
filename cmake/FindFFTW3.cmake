# - Find FFTW3
# Find the native FFTW-3 includes and library
#
#  FFTW3_INCLUDE_DIRS    - where to find fftw3.h
#  FFTW3_LIBRARIES   - List of libraries when using FFTW.
#  FFTW3_FOUND       - True if FFTW found.

if(FFTW3_INCLUDE_DIRS)
  # Already in cache, be silent
  set(FFTW3_FIND_QUIETLY TRUE)
endif(FFTW3_INCLUDE_DIRS)

find_path(FFTW3_INCLUDE_DIRS fftw3.h)
find_library(FFTW3_LIBRARIES NAMES fftw3)

# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3 DEFAULT_MSG FFTW3_LIBRARIES FFTW3_INCLUDE_DIRS)

mark_as_advanced(FFTW3_LIBRARIES FFTW3_INCLUDE_DIRS)
