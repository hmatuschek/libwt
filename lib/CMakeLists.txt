SET(WT_SOURCES
    object.cc exception.cc fft_fftw3.cc convolution.cc wavelet.cc waveletanalysis.cc
    wavelettransform.cc waveletsynthesis.cc)
SET(WT_ARRAY_HEADERS
    array/arrayobject.hh array/arrayiterator.hh array/arrayvalues.hh array/array.hh
    array/operators.hh array/dataptr.hh)
SET(WT_HEADERS
    wt.hh fft.hh types.hh
    object.hh exception.hh fft_fftw3.hh convolution.hh wavelet.hh waveletanalysis.hh
    wavelettransform.hh waveletsynthesis.hh)

add_custom_target(wt_headers SOURCES ${WT_HEADERS})

add_library(libwt SHARED ${WT_SOURCES})
set_target_properties(libwt PROPERTIES OUTPUT_NAME wt)
set_target_properties(libwt PROPERTIES DEPENDS wt_headers)
set_target_properties(libwt PROPERTIES VERSION
 "${libwt_VERSION_MAJOR}.${libwt_VERSION_MINOR}.${libwt_VERSION_PATCH}")
set_target_properties(libwt PROPERTIES SOVERION "${libwt_VERSION_MAJOR}")
set_target_properties(libwt PROPERTIES MACOSX_RPATH "${CMAKE_INSTALL_RPATH}")
target_link_libraries(libwt ${LIBS})


install(TARGETS libwt DESTINATION "${CMAKE_INSTALL_LIBDIR}")
INSTALL_HEADERS_WITH_DIRECTORY("${LIBSDR_HEADERS}" "${CMAKE_INSTALL_INCLUDEDIR}/libwt")
#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.hh"
#        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libwt")

