project(knossos)

cmake_minimum_required(VERSION 2.8.9)

add_definitions(-DREVISION="\\"git-svn\\"")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CURL REQUIRED)

include_directories(
    ${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIR}
)

find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Help REQUIRED)
find_package(Qt5Widgets REQUIRED)

SET(KNOSSOS_SOURCES
    widgets/mainwindow.cpp
    eventmodel.cpp
    client.cpp
    loader.cpp
    viewer.cpp
    remote.cpp
    skeletonizer.cpp
    renderer.cpp
    knossos.cpp
    coordinate.cpp
    Hashtable.cpp
    sleeper.cpp
    widgets/viewport.cpp
    treeLUT_fallback.cpp
    widgets/console.cpp
    widgets/tracingtimewidget.cpp
    widgets/commentswidget.cpp
    widgets/commentshortcuts/commentshortcutstab.cpp
    widgets/commentshortcuts/commentshighlightingtab.cpp
    widgets/zoomandmultireswidget.cpp
    widgets/datasavingwidget.cpp
    widgets/navigationwidget.cpp
    widgets/viewportsettingswidget.cpp
    widgets/toolswidget.cpp
    widgets/tools/toolsquicktabwidget.cpp
    widgets/tools/toolstreestabwidget.cpp
    widgets/tools/toolsnodestabwidget.cpp
    widgets/viewportsettings/vpsliceplaneviewportwidget.cpp
    widgets/viewportsettings/vpskeletonviewportwidget.cpp
    widgets/viewportsettings/vpgeneraltabwidget.cpp
    widgets/synchronizationwidget.cpp
    widgets/splashscreenwidget.cpp
    widgets/datasetpropertywidget.cpp
    widgets/task/taskloginwidget.cpp
    widgets/task/taskmanagementwidget.cpp
    widgets/task/taskmanagementmaintab.cpp
    widgets/task/taskmanagementdetailstab.cpp
    functions.cpp
    widgets/widgetcontainer.cpp
    decorators/skeletonizerdecorator.cpp
    decorators/mainwindowdecorator.cpp
    widgets/commentshortcuts/commentsnodecommentstab.cpp
    scripting.cpp
    qsort.cpp
    ftp.cpp
    task.cpp
    openjpeg/cio.c
    openjpeg/bio.c
    openjpeg/color.c
    openjpeg/convert.c
    openjpeg/dwt.c
    openjpeg/event.c
    openjpeg/function_list.c
    openjpeg/image.c
    openjpeg/index.c
    openjpeg/invert.c
    openjpeg/j2k.c
    openjpeg/jp2.c
    openjpeg/mct.c
    openjpeg/mqc.c
    openjpeg/openjpeg.c
    openjpeg/opj_clock.c
    openjpeg/opj_decompress.c
    openjpeg/opj_getopt.c
    openjpeg/pi.c
    openjpeg/raw.c
    openjpeg/t1.c
    openjpeg/t2.c
    openjpeg/tcd.c
    openjpeg/tgt.c
    test/testcommentswidget.cpp
    test/testskeletonviewport.cpp
    test/testtoolswidget.cpp
    test/testnavigationwidget.cpp
    test/testzoomandmultireswidget.cpp
    test/testorthogonalviewport.cpp
    test/testdatasavingwidget.cpp
    test/testskeletonloadandsave.cpp
    test/knossostestrunner.cpp
    sha256.cpp
    widgets/documentationwidget.cpp
    widgets/annotationwidget.cpp
    widgets/tools/toolscommandstab.cpp
    widgets/tools/toolstreeviewtab.cpp
    stateInfo.cpp
)

set(KNOSSOS_UIS
    mainwindow.ui
)

qt5_add_resources(KNOSSOS_RESOURCES_SOURCES
    Resources.qrc
)

qt5_generate_moc(knossos-global.h ${CMAKE_CURRENT_BINARY_DIR}/moc_knossos-global.cpp)

add_executable(knossos
    ${KNOSSOS_SOURCES}
    ${KNOSSOS_RESOURCES_SOURCES}
    ${KNOSSOS_UIS}
    ${CMAKE_CURRENT_BINARY_DIR}/moc_knossos-global.cpp
)

target_link_libraries(knossos
    ${OPENGL_LIBRARIES}
    ${CURL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

qt5_use_modules(knossos OpenGL Network Xml Test Help Widgets)

qt5_wrap_ui(KNOSSOS_UIS_HEADERS
    ${KNOSSOS_UIS}
)

