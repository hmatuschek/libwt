SET(wtool_SOURCES main.cc fmtutil.cc polygon.cc session.cc
    item.cc timeseriesitem.cc transformeditem.cc transformitem.cc synthesisitem.cc projectionitem.cc
    itemview.cc mainwindow.cc application.cc transformdialog.cc qcustomplot.cc timeseriesplot.cc
    transformedplot.cc procinfo.cc loghandler.cc)
SET(wtool_MOC_HEADERS
    item.hh timeseriesitem.hh transformeditem.hh transformitem.hh synthesisitem.hh projectionitem.hh
    itemview.hh mainwindow.hh application.hh transformdialog.hh qcustomplot.hh timeseriesplot.hh
    transformedplot.hh procinfo.hh loghandler.hh)
SET(wtool_HEADERS ${wtool_MOC_HEADERS}
    fmtutil.hh polygon.hh session.hh)

#qt5_wrap_cpp(wtool_MOC_SOURCES ${wtool_MOC_HEADERS})
set(CMAKE_AUTOMOC ON)
qt5_add_resources(wtool_RCC_SOURCES resources/resources.qrc)

add_executable(wtool ${wtool_SOURCES} ${wtool_MOC_SOURCES} ${wtool_RCC_SOURCES})
set(Qt_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})
target_link_libraries(wtool ${EIGEN3_LIBRARIES} ${Qt_LIBRARIES} ${HDF_LIBRARIES} libwt)
