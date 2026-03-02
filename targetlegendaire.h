#ifndef TARGETLEGENDAIRE_H
#define TARGETLEGENDAIRE_H

#include "target.h"
#include <QVector>
#include <random>
#include "modejeu.h"

class TargetLegendaire : public Target
{
public:
	TargetLegendaire(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
	static QString choisirSpriteAleatoire(ModeJeu mode);

	static const QVector<QString> spritesPlus18;
	static const QVector<QString> spritesMoins18;
};

#endif
#pragma once
