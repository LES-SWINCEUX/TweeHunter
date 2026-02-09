#ifndef MENU_PRINCIPAL_H
#define MENU_PRINCIPAL_H

#include "panneau_principal.h"
#include "decoration_menu.h"

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QElapsedTimer>
#include <QResizeEvent>
#include <iostream>

class MenuPrincipal : public QWidget
{
    Q_OBJECT

public:
    MenuPrincipal(QWidget* parent = nullptr);
    ~MenuPrincipal();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QPixmap arrierePlan;
    QPixmap arrierePlanCache;
    QPixmap titreSprite;

    QTimer timerAnimationTitre;

    PanneauMenu* panneau;
    DecorationMenu* cannettes = nullptr;

    const float echelleTitre = 0.55f;
    const int positionTitreY = -100;
    const int nombreImageTitre = 12;
    const int tempsAnimation = 1000;
    const int tempsAttenteAnimation = 9000;
    const float ratioPanneaux = 0.25f;

    int indexImageTitre = 0;
    int imagesAffichees = 0;
    bool animationActive = true;
    QElapsedTimer timerPauseAnimation;

    void configuerAnimationTitre();
    void afficherArrierePlan(QPainter &painter);
    void afficherTitre(QPainter &painter);
    void reconstruireCacheArrierePlan();
};

#endif