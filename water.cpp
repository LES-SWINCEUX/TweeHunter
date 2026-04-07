#include "water.h"
#include "jeu.h"

const QVector<QString> Water::spritesPlus18 = {
    "/images/sprites/water.png",
};

const QVector<QString> Water::spritesMoins18 = {
    "/images/sprites/water.png",
};

QString Water::choisirSprite(ModeJeu mode)
{
    static std::mt19937 gen(std::random_device{}());

    const QVector<QString>* liste = nullptr;
    switch (mode) {
    case ModeJeu::PLUS_18:
        liste = &spritesPlus18;
        break;
    case ModeJeu::MOINS_18:
        liste = &spritesMoins18;
        break;
    }
    std::uniform_int_distribution<int> dist(0, liste->size() - 1);
    return (*liste)[dist(gen)];
}

Water::Water(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode)
    : Target(choisirSprite(mode), 4, 3, 800, mouvement, TypeTarget::WATER, taille)
{
    setPointsScore(0);
}