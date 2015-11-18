SET(WT_UTILS_SOURCES csv.cc option_parser.cc unittest.cc cputime.cc)
SET(WT_UTILS_HEADERS csv.hh option_parser.hh unittest.hh cputime.hh)

add_custom_target(wt_utils_headers SOURCES ${WT_UTILS_HEADERS})

add_library(libwtutils SHARED ${WT_UTILS_SOURCES})
set_target_properties(libwtutils PROPERTIES OUTPUT_NAME wt-utils)
set_target_properties(libwtutils PROPERTIES DEPENDS wt_uitls_headers)
set_target_properties(libwtutils PROPERTIES VERSION
 "${libwt_VERSION_MAJOR}.${libwt_VERSION_MINOR}.${libwt_VERSION_PATCH}")
set_target_properties(libwtutils PROPERTIES SOVERION "${libwt_VERSION_MAJOR}")
set_target_properties(libwtutils PROPERTIES MACOSX_RPATH "${CMAKE_INSTALL_RPATH}")
set_target_properties(libwtutils PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_RPATH})
target_link_libraries(libwtutils ${LIBS})


install(TARGETS libwtutils DESTINATION "${CMAKE_INSTALL_LIBDIR}")
INSTALL_HEADERS_WITH_DIRECTORY("${LIBSDR_HEADERS}" "${CMAKE_INSTALL_INCLUDEDIR}/libwt/uitls")

