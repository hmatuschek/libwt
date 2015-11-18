SET(UTILS_SOURCES
    utils/cputime.cc utils/unittest.cc utils/option_parser.cc utils/csv.cc)
SET(UTILS_HEADERS
    utils/cputime.cc utils/unittest.cc utils/option_parser.cc utils/csv.hh)

SET(WT_SOURCES
    object.cc fft_fftw3.cc convolution.cc wavelet.cc waveletanalysis.cc
    wavelettransform.cc waveletsynthesis.cc waveletconvolution.cc api.cc)
SET(WT_HEADERS
    wt.hh fft.hh types.hh movingaverage.hh
    object.hh fft_fftw3.hh convolution.hh wavelet.hh waveletanalysis.hh
    wavelettransform.hh waveletsynthesis.hh waveletconvolution.hh api.hh)

add_custom_target(wt_headers SOURCES ${WT_HEADERS} ${UTILS_HEADERS})

add_library(libwt SHARED ${WT_SOURCES} ${UTILS_SOURCES})
set_target_properties(libwt PROPERTIES OUTPUT_NAME wt)
set_target_properties(libwt PROPERTIES DEPENDS wt_headers)
set_target_properties(libwt PROPERTIES VERSION
 "${libwt_VERSION_MAJOR}.${libwt_VERSION_MINOR}.${libwt_VERSION_PATCH}")
set_target_properties(libwt PROPERTIES SOVERION ${libwt_VERSION_MAJOR})
set_target_properties(libwt PROPERTIES MACOSX_RPATH ${CMAKE_INSTALL_RPATH})
set_target_properties(libwt PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_RPATH})
target_link_libraries(libwt ${LIBS})


install(TARGETS libwt DESTINATION "${CMAKE_INSTALL_LIBDIR}")
INSTALL_HEADERS_WITH_DIRECTORY("${WT_HEADERS}" "${CMAKE_INSTALL_INCLUDEDIR}/libwt")
#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.hh"
#        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libwt")

