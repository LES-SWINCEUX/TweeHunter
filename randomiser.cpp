#include "randomiser.h"

Randomiser::Randomiser(const QSizeF& tailleEcran)
	: tailleEcran(tailleEcran), intervalSpawn(INTERVALLE_SPAWN_DEFAUT), variationSpawn(VARIATION_SPAWN_DEFAUT), margeEcran(MARGE_ECRAN_DEFAULT), generateur(std::random_device{}())
{
}

void Randomiser::ajouterTypeTarget(const DefinitionTarget& definition)
{
	typesDisponibles.append(definition);
}

bool Randomiser::doitGenererTarget(qint64 tempsMs)
{
	if (prochainSpawn == 0) {
		prochainSpawn = tempsMs + intervalSpawn;
		return false;
	}
	if (tempsMs >= prochainSpawn) {
		std::uniform_int_distribution<qint64> dist(-variationSpawn, variationSpawn);
		prochainSpawn = tempsMs + intervalSpawn + dist(generateur);
		return true;
	}
	return false;
}

Target* Randomiser::genererTarget()
{
	if (typesDisponibles.isEmpty()) {
		return nullptr;
	}
	std::uniform_int_distribution<int> distType(0, typesDisponibles.size() - 1);
	const DefinitionTarget& def = typesDisponibles[distType(generateur)];

	Bord bordDepart = choisirBordAleatoire();
	QPointF pointDepart = choisirPointDepart(bordDepart);
	Bord bordArrivee = choisirBordOppose(bordDepart);
	QPointF pointArrivee = choisirPointArrivee(bordArrivee);

	Mouvement* mouvement = new Mouvement(pointDepart, pointArrivee, choisirVitesse(def.vitesseMin, def.vitesseMax), choisirTrajectoire());

	Target* cible = new Target(def.cheminSprite, def.colonnesSprite, def.lignesSprite, def.cycleAnimation, mouvement, def.type, def.taille);
	cible->setPointsScore(def.pointsScore);
	if (bordDepart == Bord::DROITE) {
		cible->setMiroir(true);
	}
	return cible;
}

QPointF Randomiser::choisirPointDepart(Bord bord) const
{
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	const double depassement = 200.0;
	const double margeCoins = 150.0;

	switch (bord) {
	case Bord::GAUCHE:
		return QPointF(-depassement, margeCoins + dist(generateur) * (tailleEcran.height() - 2 * margeCoins - 300.0));
	case Bord::DROITE:
		return QPointF(tailleEcran.width() + depassement, margeCoins + dist(generateur) * (tailleEcran.height() - 2 * margeCoins - 300.0));
	}
	return QPointF(0, 0);
}

QPointF Randomiser::choisirPointArrivee(Bord bord) const
{
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	const double depassement = 200.0;
	const double margeCoins = 150.0;

	switch (bord) {
	case Bord::GAUCHE:
		return QPointF(-depassement, margeCoins + dist(generateur) * (tailleEcran.height() - 2 * margeCoins - 300.0));
	case Bord::DROITE:
		return QPointF(tailleEcran.width() + depassement, margeCoins + dist(generateur) * (tailleEcran.height() - 2 * margeCoins - 300.0));
	}
	return QPointF(0, 0);
}

Bord Randomiser::choisirBordAleatoire() const
{
	std::uniform_int_distribution<int> dist(0, 1);
	switch (dist(generateur)) {
	case 0: return Bord::GAUCHE;
	case 1: return Bord::DROITE;
	}
	return Bord::GAUCHE;
}

Bord Randomiser::choisirBordOppose(Bord bordDepart) const
{
	switch (bordDepart) {
	case Bord::GAUCHE:
		return Bord::DROITE;
	case Bord::DROITE:
		return Bord::GAUCHE;
	}
	return Bord::GAUCHE;
}

double Randomiser::choisirVitesse(double min, double max) const
{
	std::uniform_real_distribution<double> dist(min, max);
	return dist(generateur);
}

TypeTrajectoire Randomiser::choisirTrajectoire() const
{
	std::uniform_int_distribution<int> dist(0, 9);
	int choix = dist(generateur);

	if (choix < 2)
		return TypeTrajectoire::LINEAIRE;
	if (choix < 4)
		return TypeTrajectoire::COURBE_HAUT;
	if (choix < 6) 
		return TypeTrajectoire::COURBE_BAS;
	return TypeTrajectoire::ZIGZAG;
}