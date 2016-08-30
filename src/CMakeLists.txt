SET(wtool_SOURCES main.cc )

add_executable(wtool ${wtool_SOURCES})
target_link_libraries(wtool ${EIGEN3_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} libwt)
