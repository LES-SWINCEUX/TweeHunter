#ifndef COMPTEUR_POINTS_H
#define COMPTEUR_POINTS_H

#include <QWidget>
#include <QPixmap>
#include <QFont>
#include <QTimer>
#include <QPainter>
#include <algorithm>

#include "sprite_manager.h"

class CompteurPoints : public QWidget
{
    Q_OBJECT
public:
    CompteurPoints(QWidget* parent = nullptr);
    void setNombresNumeros(int count);

    void setPoints(int value);
    int getPoints() const { return points; }

    void setAnimation(bool on);
    void setVitesseAnimation(int pointsPerTick, int tickMs = 16);

    void setEchelle(qreal s);
    qreal getEchelle() const { return echelle; }

    QSize basePanelSize() const { return tailleSprite; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QString formatterPoints(int v) const;
    void setStyleEcriture();
    void setSpritesheet();
    void recalculerTaille();

    QSharedPointer<QPixmap> spriteSheet;
    QSize tailleSprite;

    QFont styleEcriture;
    int nombreNumero = 4;

    int points = 0;
    int pointsCible = 0;

    float echelle = 1.0;

    bool estAnime = false;
    int tempsParPoint = 50;
    QTimer timerAnimation;
};

#endif