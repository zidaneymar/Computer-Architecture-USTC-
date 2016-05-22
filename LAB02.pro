#-------------------------------------------------
#
# Project created by QtCreator 2016-05-13T21:45:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LAB02
TEMPLATE = app


SOURCES +=\
        widget.cpp \
    cachesimulation.cpp \
    qchartviewer.cpp \
    trackaxis.cpp

HEADERS  += widget.h \
    cachesimulation.h \
    qchartviewer.h \
    trackaxis.h

FORMS    += widget.ui


INCLUDEPATH += /Users/kangqi/Desktop/LAB02/include

DEFINES += CHARTDIR_HIDE_OBSOLETE _CRT_SECURE_NO_WARNINGS


macx:LIBS += -L /Users/kangqi/Desktop/LAB02/lib -lchartdir
macx:QMAKE_POST_LINK += mkdir -p \"`dirname $(TARGET)`/../Frameworks\";
macx:QMAKE_POST_LINK += cp /Users/kangqi/Desktop/LAB02/lib/libchartdir.6.dylib \"`dirname $(TARGET)`/../Frameworks\";
macx:QMAKE_POST_LINK += install_name_tool -change libchartdir.6.dylib \
     \@loader_path/../Frameworks/libchartdir.6.dylib \"$(TARGET)\";
