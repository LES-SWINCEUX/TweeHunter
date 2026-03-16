#include "jeu.h"

static const QString CHEMIN_DESTRUCTION = "/images/sprites/Explosion.png";
static constexpr int COLONNES_DESTRUCTION = 4;
static constexpr int LIGNES_DESTRUCTION = 3;
static constexpr int CYCLE_DESTRUCTION = 1000;

QSharedPointer<QPixmap> Jeu::spriteDestruction = nullptr;

Jeu::Jeu(const QSizeF& tailleEcran, CompteurPoints* compteurPoints, CompteurBalles* compteurBalles, Vies* vies, ModeJeu mode, Armes* A)
	: randomiser(nullptr), score(0), ciblesTouchees(0), ciblesManquees(0), maxCiblesSimultanees(4), enPause(false), modeActuel(mode)
{
	armes = A;

	if (!spriteDestruction) {
		QString chemin = QDir::currentPath() + "/images/sprites/Explosion.png";
		spriteDestruction = SpriteManager::instance().getPixmap(chemin);
		//Explosion(Mouvement::LINEAIRE, tempsMs, 25);
	}
	randomiser = new Randomiser(tailleEcran);

	randomiser->setFrequenceSpawn(1000);
	randomiser->setVariationFrequence(500);
	randomiser->setMarge(20.0);

	this->compteurPoints = compteurPoints;
	this->compteurBalles = compteurBalles;
	this->vies = vies;

	initialiserCiblesParDefaut();
	qDebug() << QDir::currentPath();
}

Jeu::~Jeu()
{
	qDeleteAll(ciblesActives);
	ciblesActives.clear();
	delete randomiser;
}

void Jeu::update(qint64 tempsMs)
{
	if (enPause) {
		return;
	}

	bool aTouche = false;
	if (randomiser && randomiser->doitGenererTarget(tempsMs)) {
		if (ciblesActives.size() < maxCiblesSimultanees) {
			Target* nouvelleCible = randomiser->genererTarget(modeActuel);
			if (nouvelleCible) {
				ciblesActives.append(nouvelleCible);
			}
		}
	}
	
	for (Target* cible : ciblesActives) {
		if (cible) {
			cible->update(tempsMs);
			if (cible->estInactif() && !cible->dejaComptee()) {
				ciblesManquees++;
				cible->marquerComptee();
			}
		}

	}
	nettoyerCiblesInactives();
}

void Jeu::dessiner(QPainter& painter, qint64 tempsMs)
{
	for (Target* cible : ciblesActives) {
		if (cible) {
			cible->dessiner(painter, tempsMs);
		}
	}
}

bool Jeu::verifierCollisions(const QPainterPath& cercleReticule, qint64 tempsMs)
{
	if (enPause) {
		return false;
	}

	bool aTouche = false;

	for (Target* cible : ciblesActives) {
		if (cible && cible->estActif() && cible->intersecte(cercleReticule)) {
			aTouche = true;
			cible->jouerAnimationDestruction(CHEMIN_DESTRUCTION, COLONNES_DESTRUCTION, LIGNES_DESTRUCTION, CYCLE_DESTRUCTION);
			cible->detruire(tempsMs);

			int incrementScores = cible->getPointsScore();
			score += incrementScores;

			if (score <= 0) {
				score = 0;
			}

			if (incrementScores < 0) {
				vies->setDemiVies(vies->getDemiVies() - 1);
			}

			compteurPoints->setPoints(score);
			ciblesTouchees++;
		}
	}
	return aTouche;
}


void Jeu::reinitialiser()
{
	qDeleteAll(ciblesActives);
	ciblesActives.clear();

	score = 0;
	ciblesTouchees = 0;
	ciblesManquees = 0;
	enPause = false;
}

bool Jeu::Tirer(const int x, const int y, qint64 tempsMs, int Tir) {

	if (compteurBalles && compteurBalles->getBalles() <= 0) {
		return false;
	}
	if (compteurBalles) {
		compteurBalles->setBalles(compteurBalles->getBalles() - 1);
	}
	cout << "Création de la hitbox du tir avec un cercle centré sur le réticule dans la classe Arme" << endl;

	return verifierCollisions(armes->choixArme(Tir,x,y), tempsMs);
}

bool Jeu::Explosion(const int x, const int y, qint64 tempsMs, int explo) {
	cout << "Création de la hitbox de l'explosion avec un cercle centré sur la cannette" << endl;
	return verifierCollisions(armes->choixArme(explo,x,y), tempsMs);
}

void Jeu::nettoyerCiblesInactives()
{
	auto it = ciblesActives.begin();
	while (it != ciblesActives.end()) {
		if ((*it)->estInactif()) {
			delete *it;
			it = ciblesActives.erase(it);
		} 
		else {
			++it;
		}
	}
}

void Jeu::setFrequenceSpawn(qint64 intervalMs)
{
	if (randomiser) {
		randomiser->setFrequenceSpawn(intervalMs);
	}
}

void Jeu::setVariationFrequence(qint64 variationMs)
{
	if (randomiser) {
		randomiser->setVariationFrequence(variationMs);
	}
}

void Jeu::setMaxCiblesSimultanees(int max)
{
	maxCiblesSimultanees = qMax(1, max);
}

void Jeu::setTailleEcran(const QSizeF& taille)
{

	if (randomiser) {
		randomiser->setTailleEcran(taille);
	}
}

void Jeu::ajouterTypeCible(const DefinitionTarget& definition)
{
	if (randomiser) {
		randomiser->ajouterTypeTarget(definition);
	}
}

void Jeu::setModeJeu(ModeJeu mode)
{
	modeActuel = mode;
}

void Jeu::initialiserCiblesParDefaut()
{
	DefinitionTarget buff;
	buff.type = TypeTarget::BUFF;
	buff.tailleRelative = 0.15;
	buff.pointsScore = 10;
	buff.vitesseMin = 500.0;
	buff.vitesseMax = 1000.0;
	buff.frequenceSpawn = 1.0;
	ajouterTypeCible(buff);

	DefinitionTarget debuff;
	debuff.type = TypeTarget::DEBUFF;
	debuff.tailleRelative = 0.20;
	debuff.pointsScore = -15;
	debuff.vitesseMin = 500.0;
	debuff.vitesseMax = 1250.0;
	debuff.frequenceSpawn = 2.0;

	ajouterTypeCible(debuff);
	DefinitionTarget mixte;
	mixte.type = TypeTarget::MIXTE;
	mixte.tailleRelative = 0.15;
	mixte.pointsScore = 20;
	mixte.vitesseMin = 500.0;
	mixte.vitesseMax = 1250.0;
	mixte.frequenceSpawn = 2.5;
	ajouterTypeCible(mixte);

	DefinitionTarget legendaire;
	legendaire.type = TypeTarget::LEGENDAIRE;
	legendaire.tailleRelative = 0.15;
	legendaire.pointsScore = 50;
	legendaire.vitesseMin = 420.0;
	legendaire.vitesseMax = 2050.0;
	legendaire.frequenceSpawn = 4.0;
	ajouterTypeCible(legendaire);

	DefinitionTarget bonus;
	bonus.type = TypeTarget::BONUS;
	bonus.tailleRelative = 0.15;
	bonus.pointsScore = 30;
	bonus.vitesseMin = 500.0;
	bonus.vitesseMax = 1250.0;
	bonus.frequenceSpawn = 5.0;
	ajouterTypeCible(bonus);
}
