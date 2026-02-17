QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

SOURCES += \
    main.cpp \
    main_window.cpp \
    menu.cpp \
    panneau_principal.cpp \
    decoration_menu.cpp \
    bouton.cpp \
    sprite.cpp \
    spritesheet.cpp \
    sprite_manager.cpp \
    ecran_jeu.cpp \
    fade_overlay.cpp \ 
    jeu.cpp \
    mouvement.cpp \
    randomiser.cpp \ 
    target.cpp \
    

HEADERS += \
    main_window.h \
    menu.h \
    panneau_menu.h \
    panneau_principal.h \
    decoration_menu.h \
    bouton.h \
    sprite.h \
    spritesheet.h \
    sprite_manager.h \
    ecran_jeu.h \
    fade_overlay.h \
    jeu.h \
    mouvement.h \
    randomiser.h \ 
    target.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
