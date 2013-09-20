#-------------------------------------------------
#
# Project created by QtCreator 2012-09-05T18:04:31
#
#-------------------------------------------------

QT       += core gui opengl network xml testlib

TARGET = knossos
TEMPLATE = app
CONFIG -=app_bundle

SOURCES += widgets/mainwindow.cpp \
    eventmodel.cpp \
    client.cpp \
    loader.cpp \
    viewer.cpp \
    remote.cpp \
    skeletonizer.cpp \
    renderer.cpp \
    knossos.cpp \
    coordinate.cpp \
    Hashtable.cpp \
    sleeper.cpp \
    widgets/viewport.cpp \
    treeLUT_fallback.cpp \
    widgets/console.cpp \
    widgets/tracingtimewidget.cpp \
    widgets/commentswidget.cpp \
    widgets/commentshortcuts/commentshortcutstab.cpp \
    widgets/commentshortcuts/commentspreferencestab.cpp \
    widgets/zoomandmultireswidget.cpp \
    widgets/datasavingwidget.cpp \
    widgets/navigationwidget.cpp \
    widgets/viewportsettingswidget.cpp \
    widgets/toolswidget.cpp \
    widgets/tools/toolsquicktabwidget.cpp \
    widgets/tools/toolstreestabwidget.cpp \
    widgets/tools/toolsnodestabwidget.cpp \
    widgets/viewportsettings/vpsliceplaneviewportwidget.cpp \
    widgets/viewportsettings/vpskeletonviewportwidget.cpp \
    widgets/viewportsettings/vpgeneraltabwidget.cpp \
    widgets/synchronizationwidget.cpp \
    widgets/splashscreenwidget.cpp \
    widgets/coordinatebarwidget.cpp \
    functions.cpp \
    widgets/widgetcontainer.cpp \
    decorators/skeletonizerdecorator.cpp \
    decorators/mainwindowdecorator.cpp \
    widgets/commentshortcuts/commentsnodecommentstab.cpp \
    scripting.cpp \
    qsort.cpp \
    ftp.cpp \
    openjpeg/cio.c \
    openjpeg/bio.c \
    openjpeg/color.c \
    openjpeg/convert.c \
    openjpeg/dwt.c \
    openjpeg/event.c \
    openjpeg/function_list.c \
    openjpeg/image.c \
    openjpeg/index.c \
    openjpeg/invert.c \
    openjpeg/j2k.c \
    openjpeg/jp2.c \
    openjpeg/mct.c \
    openjpeg/mqc.c \
    openjpeg/openjpeg.c \
    openjpeg/opj_clock.c \
    openjpeg/opj_decompress.c \
    openjpeg/opj_getopt.c \
    openjpeg/pi.c \
    openjpeg/raw.c \
    openjpeg/t1.c \
    openjpeg/t2.c \
    openjpeg/tcd.c \
    openjpeg/tgt.c \
    test/testcommentswidget.cpp

HEADERS  += widgets/mainwindow.h \
    knossos-global.h \
    eventmodel.h \
    client.h \
    loader.h \
    viewer.h \
    remote.h \
    skeletonizer.h \
    renderer.h \
    knossos.h\
    sleeper.h \
    widgets/viewport.h \
    widgets/console.h \
    widgets/tracingtimewidget.h \
    widgets/commentswidget.h \
    widgets/commentshortcuts/commentshortcutstab.h \
    widgets/commentshortcuts/commentspreferencestab.h \
    widgets/zoomandmultireswidget.h \
    widgets/datasavingwidget.h \
    widgets/navigationwidget.h \
    widgets/viewportsettingswidget.h \
    widgets/toolswidget.h \
    widgets/tools/toolsquicktabwidget.h \
    widgets/tools/toolstreestabwidget.h \
    widgets/tools/toolsnodestabwidget.h \
    widgets/viewportsettings/vpsliceplaneviewportwidget.h \
    widgets/viewportsettings/vpskeletonviewportwidget.h \
    widgets/viewportsettings/vpgeneraltabwidget.h \
    widgets/synchronizationwidget.h \
    widgets/splashscreenwidget.h \
    widgets/coordinatebarwidget.h \
    functions.h \
    GUIConstants.h \
    widgets/widgetcontainer.h \
    decorators/skeletonizerdecorator.h \
    decorators/mainwindowdecorator.h \
    widgets/commentshortcuts/commentsnodecommentstab.h \
    scripting.h \
    ftp.h \
    test/testcommentswidget.h \
    openjpeg/tgt.h \
    openjpeg/tcd.h \
    openjpeg/t2.h \
    openjpeg/t1_luts.h \
    openjpeg/t1.h \
    openjpeg/raw.h \
    openjpeg/pi.h \
    openjpeg/opj_stdint.h \
    openjpeg/opj_malloc.h \
    openjpeg/opj_inttypes.h \
    openjpeg/opj_intmath.h \
    openjpeg/opj_includes.h \
    openjpeg/opj_getopt.h \
    openjpeg/opj_config_private.h \
    openjpeg/opj_config.h \
    openjpeg/opj_clock.h \
    openjpeg/opj_apps_config.h \
    openjpeg/openjpeg.h \
    openjpeg/mqc.h \
    openjpeg/mct.h \
    openjpeg/jp2.h \
    openjpeg/j2k.h \
    openjpeg/invert.h \
    openjpeg/indexbox_manager.h \
    openjpeg/index.h \
    openjpeg/image.h \
    openjpeg/function_list.h \
    openjpeg/format_defs.h \
    openjpeg/event.h \
    openjpeg/dwt.h \
    openjpeg/convert.h \
    openjpeg/color.h \
    openjpeg/cio.h \
    openjpeg/cidx_manager.h \
    openjpeg/bio.h


FORMS    += mainwindow.ui

OTHER_FILES += \
    knossos.layout \
    iconv.dll \
    libfreetype-6.dll \
    pthreadVC2.dll \
    zlib1.dll \
    icon \
    LICENSE \
    Makefile \
    splash \
    knossos.depend \
    knossos.dev \
    default.lut \
    gmon.out \
    knossos.res \
    knossos_private.res \
    knossos.rc \
    knossos_private.rc \
    logo.ico \
    ChangeLog.txt \
    defaultSettings.xml \
    customCursor.xpm \
    config.y


mac {
    INCLUDEPATH += /usr/include/Python2.7 \
                   /usr/lib/
                   /usr/include
    LIBS += -framework Python \
            -lPythonQt \
            -lcurl
}

linux {
    LIBS += -lGL \
            -lGLU \
            -L/usr/lib/i386-linux-gnu/mesa/lGL \

    INCLUDEPATH += /home/knossos/Dokumente/libxml \
                   /usr/include/GL/
}

win32 {

    CONFIG(debug, debug|release) {
        DEBUG_EXT = _d
    } else {
        DEBUG_EXT =
    }

    LIBS += C:\Qt\Qt5.1.0\Tools\mingw48_32\opt\lib\python2.7\config\libpython2.7.dll.a \
            C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib\libPythonQt$${DEBUG_EXT}.a \
            C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib\libPythonQt_QtAll$${DEBUG_EXT}.a \
            C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib\libcurl.dll.a \
            -lwsock32

    INCLUDEPATH += C:\Qt\Qt5.1.0\Tools\mingw48_32\opt\include\python2.7 \
                   C:\Qt\Qt5.1.0\5.1.0\mingw48_32\include \
                   C:\Qt\Qt5.1.0\5.1.0\mingw48_32\lib
}


RESOURCES += \
    Resources.qrc

include(test/config.pri)
