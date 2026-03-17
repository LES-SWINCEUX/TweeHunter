#ifndef COMPTEUR_BALLES_H
#define COMPTEUR_BALLES_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <algorithm>

#include "sprite_manager.h"

class CompteurBalles : public QWidget
{
    Q_OBJECT
public:
    CompteurBalles(QWidget* parent = nullptr, int balle=9);
    void setBalles(int value);
    int getBalles() const { return balles; }

    void setEchelle(float s);
    float getEchelle() const { return echelle; }

    QSize frameSize() const { return tailleFrame; }
    void setMaxBalles(int max) {maxBalles = max; balles = max;}// Ajuste le nombre de balles si nécessaire

signals:
    void ballesChanged(int nbBalles);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void setSpriteSheet();
    void recalcFrameRect();
    void recalcWidgetSize();

    QSharedPointer<QPixmap> spriteSheet;
    int balles = 9;
	int maxBalles = 9;

    QSize tailleFrame;    
    QRect rectangleFrame;
    float echelle = 1.0;  
};

#endif