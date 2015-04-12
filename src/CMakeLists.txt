SET(wtcli_SOURCES wtcli.cc option_parser.cc csv.cc wtcli_gen.cc wtcli_trafo.cc)

add_executable(wtcli ${wtcli_SOURCES})
target_link_libraries(wtcli ${LIBS} libwt)
