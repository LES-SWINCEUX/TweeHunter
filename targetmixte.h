#ifndef TARGETMIXTE_H
#define TARGETMIXTE_H

#include "target.h"
#include <QVector>
#include <random>
#include "modejeu.h"

class TargetMixte : public Target
{
public:
	TargetMixte(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
	static QString choisirSpriteAleatoire(ModeJeu mode);

	static const QVector<QString> spritesPlus18;
	static const QVector<QString> spritesMoins18;
};

#endif
