QT += core gui multimedia concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

INCLUDEPATH += $$PWD/SDL3/SDL3-3.4.0/include

LIBS += -L$$PWD/SDL3/SDL3-3.4.0/lib/x64 \
        -lSDL3

win32: LIBS += -lsetupapi


SOURCES += \
    main.cpp \
    main_window.cpp \
    menu.cpp \
    panneau_menu.cpp \
    panneau_principal.cpp \
    decoration_menu.cpp \
    bouton.cpp \
    sprite.cpp \
    spritesheet.cpp \
    sprite_manager.cpp \
    ecran_jeu.cpp \
    ecran_fin_partie.cpp \
    ecran_parametres.cpp \
    panneau_options.cpp \
    volume_bouton.cpp \
    fade_overlay.cpp \
    jeu.cpp \
    mouvement.cpp \
    randomiser.cpp \
    target.cpp \
    gestionnaire_audio.cpp \
    Reticule.cpp \
    Variete.cpp \
    compteur_balles.cpp \
    compteur_points.cpp \
    vie.cpp \
    Touches.cpp \
    panneau_scores.cpp \
    gestionnaire_scores.cpp \
    texte_menu.cpp \
    targetbuff.cpp \
    targetdebuff.cpp \
    targetlegendaire.cpp \
    targetmixte.cpp \
    targetbonus.cpp \
    panneau_pause_principal.cpp \
    menu_pause_overlay.cpp \
    Armes.cpp \
    NativeSerialPort.cpp \
    bouton_options.cpp \
    gestionnaire_configuration.cpp

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
    ecran_fin_partie.h \
    ecran_parametres.h \
    fade_overlay.h \
    panneau_options.h \
    volume_bouton.h \
    jeu.h \
    mouvement.h \
    randomiser.h \
    target.h \
    gestionnaire_audio.h \
    Reticule.h \
    Variete.h \
    compteur_balles.h \
    compteur_points.h \
    vie.h \
    Touches.h \
    panneau_scores.h \
    gestionnaire_scores.h \
    texte_menu.h \
    targetbuff.h \
    targetdebuff.h \
    targetlegendaire.h \
    targetmixte.h \
    targetbonus.h \
    panneau_pause_principal.h \
    menu_pause_overlay.h \
    modejeu.h \
    Armes.h \
    NativeSerialPort.h \
    configuration_partie.h \
    bouton_options.h \
    gestionnaire_configuration.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

!exists($$PWD/SDL3) {
    win32 {
        system(powershell -Command \"Invoke-WebRequest -Uri 'https://github.com/libsdl-org/SDL/releases/download/release-3.4.0/SDL3-devel-3.4.0-VC.zip' -OutFile '$$PWD/SDL3.zip'\")
        system(powershell -Command \"Expand-Archive -Path '$$PWD/SDL3.zip' -DestinationPath '$$PWD/SDL3' -Force\")
        system(del $$shell_path($$PWD/SDL3.zip))
    }
    unix:!mac {
        system(wget -q https://github.com/libsdl-org/SDL/releases/download/release-3.4.0/SDL3-3.4.0.tar.gz -O $$PWD/SDL3.tar.gz)
        system(mkdir -p $$PWD/SDL3 && tar -xzf $$PWD/SDL3.tar.gz -C $$PWD/SDL3 --strip-components=1)
        system(rm $$PWD/SDL3.tar.gz)
    }
}

win32 {
    SDL3_DLL = $$shell_path($$PWD/SDL3/SDL3-3.4.0/lib/x64/SDL3.dll)

    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += copy /Y \"$$SDL3_DLL\" \"$$shell_path($$OUT_PWD/debug/)\"
    } else {
        QMAKE_POST_LINK += copy /Y \"$$SDL3_DLL\" \"$$shell_path($$OUT_PWD/release/)\"
    }
}

unix:!mac {
    SDL3_SO = $$PWD/SDL3/SDL3-3.4.0/lib/linux/libSDL3.so

    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/debug
    } else {
        DESTDIR = $$OUT_PWD/release
    }

    copysdl3.commands = cp -P \"$$SDL3_SO\"* \"$$DESTDIR/\"
    copysdl3.depends = FORCE
    QMAKE_EXTRA_TARGETS += copysdl3
    POST_TARGETDEPS += copysdl3

    QMAKE_LFLAGS += -Wl,-rpath,\'$$ORIGIN\'
}