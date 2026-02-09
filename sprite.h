#ifndef SPRITE_H
#define SPRITE_H

#include "spritesheet.h"

#include <QRect>
#include <QSize>
#include <QPainter>
#include <QtMath>
#include <algorithm>

class Sprite
{
public:
    void setSprite(const SpriteSheet& sheet);
    void setSprite(QSharedPointer<QPixmap> pixmap, int colonnes, int lignes);

    void setCycle(int ms);
    void setDecalageImage(int decalage);

    void setClip(int indexDebut, int nombreImages, bool boucle = true);

    bool estValide() const;

    QRect getRectangle() const;

    void dessiner(QPainter& painter, const QRect& encadre, qint64 temps, bool smooth);

private:
    QRect getRectangle(qint64 tempsMs) const;
    QRect getRectangleSprite(int indexImage) const;

    static QRect obtenirRectangleEchelle(const QRect& rectangle, const QSize& taille);

    QRect dernierRectangle;

    SpriteSheet spriteSheet;

    int cycle = 1000;
    int decalageImage = 0;

    int clipStart = 0;
    int clipCount = -1;
    bool clipLoop = true;
};

#endif