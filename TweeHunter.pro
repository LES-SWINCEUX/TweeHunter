QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

SOURCES += \
    animation_clock.cpp \
    bouton.cpp \
    decoration_menu.cpp \
    main.cpp \
    main_window.cpp \
    menu.cpp \
    panneau_principal.cpp \
    sprite.cpp

HEADERS += \
    animation_clock.h \
    bouton.h \
    decoration_menu.h \
    main_window.h \
    menu.h \
    panneau_menu.h \
    panneau_principal.h \
    sprite.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
