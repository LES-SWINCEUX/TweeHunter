#include "sprite.h"

void Sprite::setSprite(const SpriteSheet& sheet)
{
    spriteSheet = sheet;
}

void Sprite::setSprite(QSharedPointer<QPixmap> pixmap, int colonnes, int lignes)
{
    spriteSheet = SpriteSheet(std::move(pixmap), colonnes, lignes);
}

void Sprite::setCycle(int ms)
{
    cycle = std::max(1, ms);
}

void Sprite::setDecalageImage(int decalage)
{
    decalageImage = decalage;
}

void Sprite::setClip(int indexDebut, int nombreImages, bool boucle)
{
    clipStart = std::max(0, indexDebut);
    clipCount = nombreImages;
    clipLoop = boucle;
}

bool Sprite::estValide() const
{
    return spriteSheet.estValide();
}

QRect Sprite::getRectangleSprite(int indexImage) const
{
    if (!estValide()) {
        return QRect();
    }
    return spriteSheet.rectangeImage(indexImage);
}

QRect Sprite::getRectangle() const
{
    return dernierRectangle;
}

QRect Sprite::getRectangle(qint64 tempsMs) const
{
    if (!estValide()) {
        return QRect();
    }

    const int totalSheet = spriteSheet.nombreImages();
    const int count = (clipCount > 0) ? std::min(clipCount, totalSheet - clipStart) : totalSheet;
    if (count <= 0) {
        return QRect();
    }

    const int frameBase = int((tempsMs % cycle) * count / cycle);
    int frame = frameBase + decalageImage;

    if (clipLoop) {
        frame %= count;
        if (frame < 0) frame += count;
    } else {
        frame = std::clamp(frame, 0, count - 1);
    }

    return getRectangleSprite(clipStart + frame);
}

QRect Sprite::obtenirRectangleEchelle(const QRect& rectangle, const QSize& taille)
{
    if (!rectangle.isValid() || !taille.isValid()) {
        return rectangle;
    }

    const double srcW = taille.width();
    const double srcH = taille.height();
    const double boxW = rectangle.width();
    const double boxH = rectangle.height();

    const double scale = std::min(boxW / srcW, boxH / srcH);
    const int w = int(srcW * scale);
    const int h = int(srcH * scale);

    const int x = rectangle.x() + (rectangle.width() - w) / 2;
    const int y = rectangle.y() + (rectangle.height() - h) / 2;
    return QRect(x, y, w, h);
}

void Sprite::dessiner(QPainter& painter, const QRect& encadre, qint64 temps, bool smooth)
{
    if (!estValide()) {
        return;
    }

    const QRect src = getRectangle(temps);
    const QRect dest = obtenirRectangleEchelle(encadre, src.size());

    dernierRectangle = dest;

    if (smooth) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }
    else {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    }

    painter.drawPixmap(dest, spriteSheet.pixmap(), src);
}
