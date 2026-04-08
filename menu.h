#ifndef MENU_PRINCIPAL_H
#define MENU_PRINCIPAL_H

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

#include "Touches.h"
#include "panneau_principal.h"
#include "panneau_options.h"
#include "decoration_menu.h"
#include "sprite_manager.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "panneau_scores.h"

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
    bool manetteConnectee() const;
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
    const int intervalleFrameTitreMs = 55; // ~18 fps pour l'animation du titre

    const float ratioPanneaux = 0.25f;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;
    bool cacherTitre = false;

    Touches* touches = nullptr;
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
};

#endif