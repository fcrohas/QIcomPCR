TEMPLATE = app

QT += qml quick widgets printsupport

SOURCES += main.cpp \
    qcustomplot.cpp \
    CustomPlotItem.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    qcustomplot.h \
    CustomPlotItem.h
