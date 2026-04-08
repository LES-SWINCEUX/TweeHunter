#ifndef WATER_H
#define WATER_H

#include "target.h"
#include <QVector>
#include <random>

class Water : public Target
{
public:
    Water(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
    QString choisirSprite(ModeJeu mode);

    static const QVector<QString> spritesPlus18;
    static const QVector<QString> spritesMoins18;
};

#endif
