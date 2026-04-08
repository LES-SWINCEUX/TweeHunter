QT += core gui multimedia concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

# SDL3 - Téléchargement automatique si absent
win32:!exists($$PWD/SDL3/SDL3-3.4.0/include/SDL3/SDL.h) {
    message("SDL3 introuvable, téléchargement en cours...")
    system(powershell -Command \"Invoke-WebRequest -Uri 'https://github.com/libsdl-org/SDL/releases/download/release-3.4.0/SDL3-devel-3.4.0-VC.zip' -OutFile '$$PWD/SDL3.zip'\")
    system(powershell -Command \"Expand-Archive -Path '$$PWD/SDL3.zip' -DestinationPath '$$PWD/SDL3' -Force\")
    system(powershell -Command \"Remove-Item '$$PWD/SDL3.zip'\")
}

SDL3_DIR = $$PWD/SDL3/SDL3-3.4.0

INCLUDEPATH += $$SDL3_DIR/include

win32 {
    contains(QT_ARCH, x86_64) {
        SDL3_LIB_DIR = $$SDL3_DIR/lib/x64
    } else {
        SDL3_LIB_DIR = $$SDL3_DIR/lib/x86
    }

    LIBS += -L$$SDL3_LIB_DIR -lSDL3 -lsetupapi

    SDL3_DLL = $$shell_path($$SDL3_LIB_DIR/SDL3.dll)
    CONFIG(debug, debug|release): OUT = $$shell_path($$OUT_PWD/debug/)
    else:                          OUT = $$shell_path($$OUT_PWD/release/)
    QMAKE_POST_LINK += copy /Y \"$$SDL3_DLL\" \"$$OUT\"
}

unix:!mac {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl3
}

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
    ecran_parametres_layout.cpp \
    ecran_parametres_navigation.cpp \
    ecran_regles.cpp \
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
    compteur_powerup.cpp \
    compteur_points.cpp \
    compteur_vies.cpp \
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
    gestionnaire_configuration.cpp \
    bush_louche.cpp \
    bush.cpp \
    gator.cpp \
    water.cpp \
    poison.cpp \
    bush.cpp

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
    ecran_regles.h \
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
    compteur_powerup.h \
    compteur_points.h \
    compteur_vies.h \
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
    bush_louche.h \
    bush.h \
    Armes.h \
    NativeSerialPort.h \
    configuration_partie.h \
    bouton_options.h \
    gator.h \
    water.h \
    poison.h \
    gestionnaire_configuration.h \
    arriere_plan.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target