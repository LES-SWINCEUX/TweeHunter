#include "randomiser.h"

Randomiser::Randomiser(const QSizeF& tailleEcran)
	: tailleEcran(tailleEcran), 
	intervalSpawn(INTERVALLE_SPAWN_DEFAUT), 
	variationSpawn(VARIATION_SPAWN_DEFAUT), 
	margeEcran(MARGE_ECRAN_DEFAULT), 
	generateur(std::random_device{}()) {}

void Randomiser::ajouterTypeTarget(const DefinitionTarget& definition)
{
	typesDisponibles.append(definition);
}

bool Randomiser::doitGenererTarget(qint64 tempsMs)
{
	tempsCourant = tempsMs;
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

Target* Randomiser::genererTarget(ModeJeu mode)
{
	if (typesDisponibles.isEmpty()) {
		return nullptr;
	}
	std::vector<double> poids;
	for (const auto& def : typesDisponibles) {
		poids.push_back(1.0 / def.frequenceSpawn);
	}
	std::discrete_distribution<int> distType(poids.begin(), poids.end());
	const DefinitionTarget& def = typesDisponibles[distType(generateur)];

	Bord bordDepart = choisirBordAleatoire();
	QPointF pointDepart = choisirPointDepart(bordDepart);
	Bord bordArrivee = choisirBordOppose(bordDepart);
	QPointF pointArrivee = choisirPointArrivee(bordArrivee);
	TypeTrajectoire traj = choisirTrajectoire();

	double facteur = calculerFacteurVitesse(tempsCourant) * facteurVitesse;
	Mouvement* mouvement = new Mouvement(pointDepart, pointArrivee, choisirVitesse(def.vitesseMin * facteur, def.vitesseMax * facteur), traj);

	QSizeF taillePixels(tailleEcran.width() * def.tailleRelative, tailleEcran.height() * def.tailleRelative);

	Target* cible = nullptr;
	switch (def.type) {
		case TypeTarget::BUFF:
			cible = new TargetBuff(mouvement, taillePixels, mode);
			break;
		case TypeTarget::DEBUFF:
			cible = new TargetDebuff(mouvement, taillePixels, mode);
			break;
		case TypeTarget::MIXTE:
			cible = new TargetMixte(mouvement, taillePixels, mode);
			break;
		case TypeTarget::LEGENDAIRE:
			cible = new TargetLegendaire(mouvement, taillePixels, mode);
			break;
		case TypeTarget::BONUS:
			cible = new TargetBonus(mouvement, taillePixels, mode);
			break;
		case TypeTarget::POISON:
			cible = new Poison(mouvement, taillePixels, mode);
			break;
		case TypeTarget::WATER:
			cible = new Water(mouvement, taillePixels, mode);
			break;
		case TypeTarget::GATOR:
			cible = new Gator(mouvement, taillePixels, mode);
			break;
		default:
			delete mouvement;
			return nullptr;
	}
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

	if (choix < 2) {
		return TypeTrajectoire::LINEAIRE;
	}
		
	if (choix < 4) {
		return TypeTrajectoire::COURBE_HAUT;
	}

	if (choix < 6) {
		return TypeTrajectoire::COURBE_BAS;
	}
		
	return TypeTrajectoire::ZIGZAG;
}

bool Randomiser::genererBushLouche(qint64 tempsMs)
{
	if (prochainBushLouche == 0) {
		std::uniform_int_distribution<qint64> dist(-VARIATION_SPAWN_BUSH_LOUCHE, VARIATION_SPAWN_BUSH_LOUCHE);
		prochainBushLouche = tempsMs + INTERVALLE_SPAWN_BUSH_LOUCHE + dist(generateur);
		return false;
	}

	if (tempsMs >= prochainBushLouche) {
		std::uniform_int_distribution<qint64> dist(-VARIATION_SPAWN_BUSH_LOUCHE, VARIATION_SPAWN_BUSH_LOUCHE);
		prochainBushLouche = tempsMs + INTERVALLE_SPAWN_BUSH_LOUCHE + dist(generateur);
		return true;
	}

	return false;
}

TypeLouche Randomiser::choisirTypeBushLouche() const
{
	std::uniform_int_distribution<int> dist(0, 2);
	return static_cast<TypeLouche>(dist(generateur));
}

int Randomiser::choisirIndexBush(int nombreBush) const
{
	if (nombreBush <= 0) {
		return 0;
	}
	std::uniform_int_distribution<int> dist(0, nombreBush - 1);
	return dist(generateur);
}

double Randomiser::calculerFacteurVitesse(qint64 tempsMs) const
{
	int palier = tempsMs / INTERVALLE_DIFFICULTE;
	double facteur = qPow(MULTIPLICATEUR_PALIER, palier);
	return qMin(facteur, FACTEUR_MAX);
}

bool Randomiser::DemarrerWave(qint64 tempsMs)
{
	if (prochaineWave == 0) {
		std::uniform_int_distribution<qint64> dist(-VARIATION_WAVE, VARIATION_WAVE);
		prochaineWave = tempsMs + INTERVALLE_WAVE + dist(generateur);
		return false;
	}

	if (enWave) {
		if (tempsMs >= prochaineWave) {
			enWave = false;
			std::uniform_int_distribution<qint64> dist(-VARIATION_WAVE, VARIATION_WAVE);
			prochaineWave = tempsMs + INTERVALLE_WAVE + dist(generateur);
		}
	}
	else {
		if (tempsMs >= prochaineWave) {
			enWave = true;
			std::uniform_int_distribution<qint64> dist(-VARIATION_DUREE_WAVE, VARIATION_DUREE_WAVE);
			prochaineWave = tempsMs + DUREE_WAVE + dist(generateur);
			return true;
		}
	}
	return false;
}