#ifndef TARGETDEBUFF_H
#define TARGETDEBUFF_H

#include "target.h"
#include <QVector>
#include <random>
#include "modejeu.h"

class TargetDebuff : public Target
{
public:
	TargetDebuff(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode);

private:
	QString choisirSpriteAleatoire(ModeJeu mode);

	static const QVector<QString> spritesPlus18;
	static const QVector<QString> spritesMoins18;
};

#endif