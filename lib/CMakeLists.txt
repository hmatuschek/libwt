SET(UTILS_SOURCES
    utils/cputime.cc utils/unittest.cc utils/option_parser.cc utils/csv.cc utils/logger.cc)
SET(UTILS_HEADERS
    utils/cputime.cc utils/unittest.cc utils/option_parser.cc utils/csv.hh utils/logger.hh)

SET(WT_SOURCES
    object.cc exception.cc fft_fftw3.cc wavelet.cc waveletanalysis.cc api.cc)
SET(WT_HEADERS
    wt.hh exception.hh fft.hh types.hh convolution.hh wavelettransform.hh waveletsynthesis.hh
    waveletconvolution.hh detrend.hh
    object.hh fft_fftw3.hh wavelet.hh waveletanalysis.hh api.hh)

if (${FFTW3_FOUND})
  message(STATUS "Using FFTW3 for FFT convolution: ${FFTW3_LIBRARIES}")
  set(WT_WITH_FFTW3 ON)
endif(${FFTW3_FOUND})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.hh.in ${CMAKE_CURRENT_BINARY_DIR}/config.hh)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(wt_headers SOURCES ${WT_HEADERS} ${UTILS_HEADERS})

add_library(libwt SHARED ${WT_SOURCES} ${UTILS_SOURCES})
set_target_properties(libwt PROPERTIES OUTPUT_NAME wt)
set_target_properties(libwt PROPERTIES VERSION
 "${libwt_VERSION_MAJOR}.${libwt_VERSION_MINOR}.${libwt_VERSION_PATCH}")
set_target_properties(libwt PROPERTIES SOVERION ${libwt_VERSION_MAJOR})
set_target_properties(libwt PROPERTIES MACOSX_RPATH ${CMAKE_INSTALL_RPATH})
set_target_properties(libwt PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_RPATH})
target_link_libraries(libwt ${LIBS})


install(TARGETS libwt DESTINATION "${CMAKE_INSTALL_LIBDIR}")
INSTALL_HEADERS_WITH_DIRECTORY("${WT_HEADERS}" "${CMAKE_INSTALL_INCLUDEDIR}/libwt")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.hh"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libwt")

