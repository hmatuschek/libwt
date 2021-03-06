IF(${INSTALL_PYTHON_USERSITE})
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -m site --user-site
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
ELSE(${INSTALL_PYTHON_USERSITE})
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF(${INSTALL_PYTHON_USERSITE})

# Get numpy header directory
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from numpy import get_include; print(get_include())"
  OUTPUT_VARIABLE NUMPY_HEADER_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
INCLUDE_DIRECTORIES(${NUMPY_HEADER_DIR})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(wt.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(wt.i PROPERTIES SWIG_FLAGS "-includeall")

SWIG_ADD_MODULE(wt python wt.i)
TARGET_LINK_LIBRARIES(_wt ${PYTHON_LIBRARIES} libwt)

install(TARGETS _wt DESTINATION ${PYTHON_SITE_PACKAGES})
install(FILES ${CMAKE_BINARY_DIR}/python/wt.py DESTINATION ${PYTHON_SITE_PACKAGES})


SET(PYWT_TEST_SOURCES test/run.py)
add_custom_target(pywt_test SOURCES ${PYWT_TEST_SOURCES})
