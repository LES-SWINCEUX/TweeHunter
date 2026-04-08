#ifndef TARGETBONUS_H
#define TARGETBONUS_H

#include "target.h"
#include <QVector>
#include <random>
#include "modejeu.h"

class TargetBonus : public Target
{
public:
	TargetBonus(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
	QString choisirSpriteAleatoire(ModeJeu mode);

	static const QVector<QString> spritesPlus18;
	static const QVector<QString> spritesMoins18;
};

#endif
#pragma once

