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

add_library(wt SHARED ${WT_SOURCES})
target_link_libraries(wt ${LIBS})
add_custom_target(wt_headers SOURCES ${WT_HEADERS} ${ARRAY_HEADERS})
