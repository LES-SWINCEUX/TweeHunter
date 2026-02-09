#include "sprite.h"

void Sprite::setSprite(const QPixmap& pixmap, int colonnes, int lignes)
{
    sprite = pixmap;
    this->colonnes = std::max(1, colonnes);
    this->lignes = std::max(1, lignes);
}

void Sprite::setCycle(int ms)
{
    cycle = std::max(1, ms);
}

void Sprite::setDecalageImage(int decalage)
{
    decalageImage = decalage;
}

bool Sprite::estValide() const
{
    return !sprite.isNull() && colonnes > 0 && lignes > 0;
}

int Sprite::nombreImages() const
{
    return colonnes * lignes;
}

QRect Sprite::getRectangleSprite(int indexImage) const
{
    if (!estValide()) {
        return QRect();
    }

    const int totalImages = nombreImages();
    indexImage %= totalImages;
    if (indexImage < 0) {
        indexImage += totalImages;
    }

    const int largeurImage = sprite.width() / colonnes;
    const int hauteurImage = sprite.height() / lignes;

    const int cx = indexImage % colonnes;
    const int cy = indexImage / colonnes;

    return QRect(cx * largeurImage, cy * hauteurImage, largeurImage, hauteurImage);
}

QRect Sprite::getRectangle(qint64 timeMs) const
{
    if (!estValide()) {
        return QRect();
    }

    const int total = nombreImages();
    int imageBase = int((timeMs % cycle) * total / cycle);
    int frame = (imageBase + decalageImage) % total;

    return getRectangleSprite(frame);
}

void Sprite::dessiner(QPainter& painter, const QRect& encadre, qint64 temps, bool smooth) const
{
    if (!estValide()) {
        return;
    }

    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);

    const QRect src = getRectangle(temps);
    if (src.isEmpty() || encadre.isEmpty()) {
        return;
    }

    const double sx = double(encadre.width()) / double(src.width());
    const double sy = double(encadre.height()) / double(src.height());
    const double s = std::min(sx, sy);

    const int w = int(std::round(src.width() * s));
    const int h = int(std::round(src.height() * s));

    const int dx = encadre.x() + (encadre.width() - w) / 2;
    const int dy = encadre.y() + (encadre.height() - h) / 2;

    const QRect dst(dx, dy, w, h);
    painter.drawPixmap(dst, sprite, src);
}