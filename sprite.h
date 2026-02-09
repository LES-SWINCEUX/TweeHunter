#ifndef SPRITE_H
#define SPRITE_H

#include <QPixmap>
#include <QRect>
#include <QSize>
#include <QPainter>
#include <QtMath>
#include <algorithm>

class Sprite
{
public:
    void setSprite(const QPixmap& pixmap, int colonnes, int lignes);
    void setCycle(int ms);
    void setDecalageImage(int decalage);

    QRect getRectangle(qint64 ms) const;

    void dessiner(QPainter& painter, const QRect& encadre, qint64 temps, bool smooth) const;

    bool estValide() const;
    int nombreImages() const;

private:
    QPixmap sprite;
    int colonnes = 1;
    int lignes = 1;
    int cycle = 1000;
    int decalageImage = 0;
    QRect getRectangleSprite(int indexImage) const;
};

#endif