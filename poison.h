#ifndef POISON_H
#define POISON_H

#include "target.h"
#include <QVector>
#include <random>

class Poison : public Target
{
public:
    Poison(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
    QString choisirSprite(ModeJeu mode);

    static const QVector<QString> spritesPlus18;
    static const QVector<QString> spritesMoins18;
};

#endif
