SET(WT_TEST_SOURCES main.cc utilstest.cc ffttest.cc convolutiontest.cc wavelettransformtest.cc
    waveletsynthesistest.cc waveletconvolutiontest.cc)

add_executable(wt_test ${WT_TEST_SOURCES})
target_link_libraries(wt_test ${LIBS} libwt)
