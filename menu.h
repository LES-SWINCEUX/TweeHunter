#ifndef MENU_PRINCIPAL_H
#define MENU_PRINCIPAL_H

#include "panneau_principal.h"
#include "panneau_options.h"
#include "decoration_menu.h"
#include "sprite_manager.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QElapsedTimer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <iostream>
#include <algorithm>
#include <SDL3/SDL.h>
#include "Touches.h"

class PanneauMenu;

class MenuPrincipal : public QWidget
{
    Q_OBJECT

public:
    MenuPrincipal(GestionnaireAudio* gestionnaireAudio, bool restartMusique, QWidget* parent = nullptr, Touches* touches = nullptr);
    ~MenuPrincipal();

signals:
    void jouerDemande();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    QSharedPointer<QPixmap> arrierePlan;
    QSharedPointer<QPixmap> titreSprite;

    QPixmap arrierePlanCache;

    QTimer timerAnimationTitre;
    QTimer timerManette;

    FadeOverlay* overlay = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    QPropertyAnimation* estompeAnimation = nullptr;
    QPropertyAnimation* fadeInMusique = nullptr;
    QPropertyAnimation* fadeInAnimation = nullptr;

    PanneauMenu* panneau = nullptr;

    DecorationMenu* cannettes = nullptr;

    const int nombreImageTitre = 12;
    const int tempsAnimation = 1000;
    const int tempsAttenteAnimation = 9000;

    const float ratioPanneaux = 0.25f;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;
    bool cacherTitre = false;

    bool dpadHautPrecedent  = false;
    bool dpadBasPrecedent   = false;
    bool boutonOkPrecedent  = false;

    SDL_Gamepad* gamepad = nullptr;

    Touches* touchesPerso   = nullptr;
    bool customHautPrecedent = false;
    bool customBasPrecedent  = false;
    bool customOkPrecedent   = false;
    bool restartMusique = false;

    QElapsedTimer timerPauseAnimation;
    QRect zonePanneauxBas() const;
    QRect zonePourPanneau(PanneauMenu* p) const;

    void configuerAnimationTitre();
    void lancerFadeIn();
    void afficherArrierePlan(QPainter& painter);
    void afficherTitre(QPainter& painter);
    void afficherOptions();
    void afficherPanneauScores();
    void afficherPanneauPrincipal();

    void initialiserManette();
    void tickManette();
};

#endif