QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += \
    widgets \
    network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectionscreen.cpp \
    fa2screen.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    securitymanager.cpp \
    signinscreen.cpp \
    signupscreen.cpp \
    welcomescreen.cpp

HEADERS += \
    basketbet.h \
    basketevent.h \
    connectionscreen.h \
    event.h \
    fa2screen.h \
    footbet.h \
    footevent.h \
    globaldata.h \
    mainwindow.h \
    networkmanager.h \
    securitymanager.h \
    signinscreen.h \
    signupscreen.h \
    welcomescreen.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

LIBS += -lcrypt
