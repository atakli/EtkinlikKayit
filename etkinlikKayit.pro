QT       += network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix{
    QMAKE_CXXFLAGS += -std=c++2a
}
win32{
    CONFIG += c++20
}

#win32:CONFIG += c++20
#unix:QMAKE_CXXFLAGS += -std=c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# --no-plugins deyince çalışmıyor. ama --no-plugins demeyince üretilen klasörlerden bi tek platforms klasörünü silince sıkıntı oluyor (içinde de sadece qwindows.dll var)
# C:\Qt-mingw\6.3.1\msvc2019_64\bin\windeployqt etkinlikKayit.exe --libdir . --plugindir . -no-translations --no-opengl-sw --no-system-d3d-compiler
# böyle bişey başına gelebilir; hatırla: Why does it miss these files? -> these files are related to the mingw runtime library, they do not belong to Qt and that's why windeployqt does not add them by default.

SOURCES += \
    activity.cpp \
    main.cpp \
    participantswidget.cpp \
    widget.cpp

HEADERS += \
    activity.h \
    participantswidget.h \
    widget.h

FORMS += \
    participantlist.ui \
    widget.ui

INCLUDEPATH += "C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\include"
win32:CONFIG(release, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\build-UpdateController-Desktop_Qt_6_4_1_MSVC2019_64bit-Release\\release\\static" -lupdatecontroller
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\build-UpdateController-Desktop_Qt_6_4_1_MSVC2019_64bit-Debug\\debug\\static" -lupdatecontroller

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
