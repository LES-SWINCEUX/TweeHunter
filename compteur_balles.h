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
    void setBalles(int value);                          // clamp 0..9
    int getBalles() const { return balles; }

    // NEW: scaling
    void setEchelle(float s);                           // ex 1.0, 1.5, 2.0...
    float getEchelle() const { return echelle; }

    QSize frameSize() const { return tailleFrame; }   // taille d’une frame (non-scalée)

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void setSpriteSheet();
    void recalcFrameRect();
    void recalcWidgetSize();

    QSharedPointer<QPixmap> spriteSheet;
    int balles = 9;

    QSize tailleFrame;    // frame size (non-scalée)
    QRect rectangleFrame;    // frame courante dans la sheet
    float echelle = 1.0;  // facteur d’échelle
};

#endif