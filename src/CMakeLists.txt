SET(wtool_SOURCES main.cc
    item.cc itemview.cc mainwindow.cc application.cc)
SET(wtool_MOC_HEADERS
    item.hh itemview.hh mainwindow.hh application.hh)
SET(wtool_HEADERS ${wtool_MOC_HEADERS})

qt5_wrap_cpp(wtool_MOC_SOURCES ${wtool_MOC_HEADERS})

add_executable(wtool ${wtool_SOURCES} ${wtool_MOC_SOURCES})

target_link_libraries(wtool ${EIGEN3_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} libwt)
