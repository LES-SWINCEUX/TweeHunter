#include "targetbuff.h"
#include "jeu.h"

const QVector<QString> TargetBuff::spritesPlus18 = {
	"/images/sprites/twisted_teas.png",
	"/images/sprites/busch_ices.png",
	"/images/sprites/heineken.png",
	"/images/sprites/pabst_blue_ribbon.png"
};

const QVector<QString> TargetBuff::spritesMoins18 = {
	"/images/sprites/coca_cola.png",
	"/images/sprites/fuze.png",
	"/images/sprites/sprite.png",
	"/images/sprites/crush.png"
};

QString TargetBuff::choisirSpriteAleatoire(ModeJeu mode)
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

TargetBuff::TargetBuff(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode)
	: Target(choisirSpriteAleatoire(mode), 4, 3, 800, mouvement, TypeTarget::BUFF, taille)
{
	setPointsScore(10);
}