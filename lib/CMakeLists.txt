SET(WT_SOURCES
    object.cc exception.cc fft_fftw3.cc convolution.cc wavelet.cc wavelettransform.cc unittest.cc
    cputime.cc)
SET(WT_ARRAY_HEADERS
    array/arrayobject.hh array/arrayiterator.hh array/arrayvalues.hh array/array.hh
    array/operators.hh)
SET(WT_HEADERS
    wt.hh fft.hh types.hh dataptr.hh
    object.hh exception.hh fft_fftw3.hh convolution.hh wavelet.hh wavelettransform.hh unittest.hh
    cputime.hh)

add_library(wt SHARED ${WT_SOURCES})
target_link_libraries(wt ${LIBS})
add_custom_target(wt_headers SOURCES ${WT_HEADERS} ${ARRAY_HEADERS})
