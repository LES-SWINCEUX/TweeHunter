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
#include <QMouseEvent>
#include <QApplication>
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
#include "easter_egg_dialog.h"

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
    void mousePressEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void configuerAnimationTitre();
    void lancerFadeIn();
    void afficherOptions();
    void afficherPanneauScores();
    void afficherPanneauPrincipal();
    void initialiserManette();

    void afficherArrierePlan(QPainter& painter);
    void afficherTitre(QPainter& painter);

    bool manetteConnectee() const;

    QRect zonePanneauxBas() const;
    QRect zonePourPanneau(PanneauMenu* p) const;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;
    bool cacherTitre = false;
    bool restartMusique = false;

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

    Touches* touches = nullptr;

    EasterEggDialog* easterEgg = nullptr;

    QElapsedTimer timerPauseAnimation;

    const int NOMBRE_IMAGE_TITRE = 12;
    const int TEMPS_ATTENTE_ANIMATION = 9000;
    const int INTERVALE_TITRE_MS = 55;

    const float RATIO_PANNEAUX = 0.25f;
};

#endif