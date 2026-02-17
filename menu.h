#ifndef MENU_PRINCIPAL_H
#define MENU_PRINCIPAL_H

#include "panneau_principal.h"
#include "panneau_options.h"
#include "decoration_menu.h"
#include "sprite_manager.h"
#include "fade_overlay.h"

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QElapsedTimer>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <iostream>
#include <algorithm>

class MenuPrincipal : public QWidget
{
    Q_OBJECT

public:
    MenuPrincipal(QWidget* parent = nullptr);
    ~MenuPrincipal();

signals:
    void jouerDemande();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QSharedPointer<QPixmap> arrierePlan;
    QSharedPointer<QPixmap> titreSprite;

    QPixmap arrierePlanCache;

    QTimer timerAnimationTitre;

    QMediaPlayer* joueurMusique = nullptr;
    QAudioOutput* musiqueMenu = nullptr;

    FadeOverlay* overlay = nullptr;

    QPropertyAnimation* estompeAnimation = nullptr;
    QPropertyAnimation* estompeMusique = nullptr;

    PanneauMenu* panneau = nullptr;

    DecorationMenu* cannettes = nullptr;

    const int nombreImageTitre = 12;
    const int tempsAnimation = 1000;
    const int tempsAttenteAnimation = 9000;

    const float ratioPanneaux = 0.25f;
    const float volumeMax = 0.025f;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;

    QElapsedTimer timerPauseAnimation;

    void configuerAnimationTitre();
    void afficherArrierePlan(QPainter &painter);
    void afficherTitre(QPainter &painter);
    void afficherOptions();
    void afficherPanneauPrincipal();
};

#endif