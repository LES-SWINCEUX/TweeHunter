#ifndef GATOR_H
#define GATOR_H

#include "target.h"
#include <QVector>
#include <random>

class Gator : public Target
{
public:
    Gator(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
    static const QVector<QString> spritesPlus18;
    static const QVector<QString> spritesMoins18;
    QString choisirSprite(ModeJeu mode);
};

#endif