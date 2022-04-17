TEMPLATE = lib
TARGET = MedianFilterPlugin
COPNFIG += plugin
INCLUDEPATH += . ../ImageEditor
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    median_filter_plugin.cpp

HEADERS += \
    median_filter_plugin.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../opencv/build/x64/vc15/lib/ -lopencv_world455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../opencv/build/x64/vc15/lib/ -lopencv_world455d

INCLUDEPATH += $$PWD/../../../../../../opencv/build/include
DEPENDPATH += $$PWD/../../../../../../opencv/build/include
