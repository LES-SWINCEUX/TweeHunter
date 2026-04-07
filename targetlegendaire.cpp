#include "targetlegendaire.h"
#include "jeu.h"

const QVector<QString> TargetLegendaire::spritesPlus18 = {
	"/images/sprites/golden_twisted_tea.png",
};

const QVector<QString> TargetLegendaire::spritesMoins18 = {
	"/images/sprites/redbull.png",
};

QString TargetLegendaire::choisirSpriteAleatoire(ModeJeu mode)
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

TargetLegendaire::TargetLegendaire(Mouvement* mouvement, const QSizeF& taille, ModeJeu mode)
	: Target(choisirSpriteAleatoire(mode), 4, 3, 800, mouvement, TypeTarget::LEGENDAIRE, taille)
{
	setPointsScore(100);
}