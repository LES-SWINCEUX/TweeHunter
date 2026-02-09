#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QPixmap>
#include <QRect>
#include <QSharedPointer>
#include <algorithm>

class SpriteSheet
{
public:
    SpriteSheet() = default;
    SpriteSheet(QSharedPointer<QPixmap> pixmap, int colonnes, int lignes);

    bool estValide() const;

    const QPixmap& pixmap() const;
    QSharedPointer<QPixmap> pixmapCache() const;

    int colonnes() const;
    int lignes() const;

    int nombreImages() const;

    QRect rectangeImage(int indexImage) const;

private:
    QSharedPointer<QPixmap> sprite;
    int nombreColonnes = 1;
    int nombreLignes = 1;
};

#endif
