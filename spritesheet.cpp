#include "spritesheet.h"

SpriteSheet::SpriteSheet(QSharedPointer<QPixmap> pixmap, int colonnes, int lignes) : 
    sprite(std::move(pixmap)), 
    nombreColonnes(std::max(1, colonnes)), 
    nombreLignes(std::max(1, lignes)) 
{
}

bool SpriteSheet::estValide() const { 
    return sprite && !sprite->isNull() && nombreColonnes > 0 && nombreLignes > 0;
}

const QPixmap& SpriteSheet::pixmap() const { 
    return *sprite;
}

QSharedPointer<QPixmap> SpriteSheet::pixmapCache() const { 
    return sprite;
}

int SpriteSheet::colonnes() const { 
    return nombreColonnes;
}

int SpriteSheet::lignes() const { 
    return nombreLignes;
}

int SpriteSheet::nombreImages() const { 
    return nombreColonnes * nombreLignes;
}

QRect SpriteSheet::rectangeImage(int indexImage) const
{
    if (!estValide()) {
        return QRect();
    }

    const int total = nombreImages();
    indexImage %= total;
    if (indexImage < 0) indexImage += total;

    const int w = sprite->width() / nombreColonnes;
    const int h = sprite->height() / nombreLignes;

    const int cx = indexImage % nombreColonnes;
    const int cy = indexImage / nombreColonnes;
    return QRect(cx * w, cy * h, w, h);
}