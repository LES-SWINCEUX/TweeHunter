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

class PanneauMenu;

class MenuPrincipal : public QWidget
{
    Q_OBJECT

public:
    MenuPrincipal(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr);
    ~MenuPrincipal();

signals:
    void jouerDemande();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;

private:
    QSharedPointer<QPixmap> arrierePlan;
    QSharedPointer<QPixmap> titreSprite;

    QPixmap arrierePlanCache;

    QTimer timerAnimationTitre;

    FadeOverlay* overlay = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
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

    QElapsedTimer timerPauseAnimation;

    // zone du bas (menu principal)
    QRect zonePanneauxBas() const;

    // zone selon le panneau affiché (scores/options = plein écran)
    QRect zonePourPanneau(PanneauMenu* p) const;

    void configuerAnimationTitre();
    void lancerFadeIn();
    void afficherArrierePlan(QPainter& painter);
    void afficherTitre(QPainter& painter);
    void afficherOptions();
    void afficherPanneauScores();
    void afficherPanneauPrincipal();
};

#endif