#ifndef VIE_H
#define VIE_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <algorithm>

#include "sprite_manager.h"

class Vies : public QWidget
{
    Q_OBJECT
public:
    Vies(QWidget* parent = nullptr);

    void setVies(int vies);
    void setDemiVies(int demiVies);
    int getDemiVies() const { return demiVies; }
    QSize getTailleFrame() const { return tailleFrame; }

    void setEchelle(float s);
    float getEchelle() const { return echelle; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    void setSpriteSheet();
    void recalculerSprite();

    QSharedPointer<QPixmap> spriteSheet;

    int maxVies = 5;
    int demiVies = 10;
    float echelle = 1.0;

    int framesSpriteSheet = 3;
    QSize tailleFrame;
    int espacementVies = 6;
};

#endif