#include "targetdebuff.h"
#include "jeu.h"

const QVector<QString> TargetDebuff::spritesPlus18 = {
	"/images/sprites/jack_daniels.png",
	//"/images/sprites/grey_goose.png",
	"/images/sprites/jimador.png",
	"/images/sprites/guiness.png",
	"/images/sprites/creme_menthe.png",
	"/images/sprites/baileys.png",
	"/images/sprites/rona.png"
};

const QVector<QString> TargetDebuff::spritesMoins18 = {
	"/images/sprites/coca_cola.png",
	"/images/sprites/fuze.png",
};

QString TargetDebuff::choisirSpriteAleatoire(ModeJeu mode)
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

TargetDebuff::TargetDebuff(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode)
	: Target(choisirSpriteAleatoire(mode), 4, 3, 800, mouvement, TypeTarget::DEBUFF, taille)
{
	setPointsScore(15);
}