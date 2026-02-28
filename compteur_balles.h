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
    CompteurBalles(QWidget* parent = nullptr);
    void setBalles(int value);
    int getBalles() const { return balles; }

    void setEchelle(float s);
    float getEchelle() const { return echelle; }

    QSize frameSize() const { return tailleFrame; }

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void setSpriteSheet();
    void recalcFrameRect();
    void recalcWidgetSize();

    QSharedPointer<QPixmap> spriteSheet;
    int balles = 9;

    QSize tailleFrame;    
    QRect rectangleFrame;
    float echelle = 1.0;  
};

#endif