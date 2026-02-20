#include "jeu.h"

static const QString CHEMIN_DESTRUCTION = "faut je trouve une image.png";
static constexpr int COLONNES_DESTRUCTION = 4;
static constexpr int LIGNES_DESTRUCTION = 3;
static constexpr int CYCLE_DESTRUCTION = 500;

Jeu::Jeu(const QSizeF& tailleEcran)
	: randomiser(nullptr), score(0), ciblesTouchees(0), ciblesManquees(0), maxCiblesSimultanees(10), enPause(false)
{
	randomiser = new Randomiser(tailleEcran);

	randomiser->setFrequenceSpawn(500);
	randomiser->setVariationFrequence(500);
	randomiser->setMarge(20.0);

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
	if (randomiser && randomiser->doitGenererTarget(tempsMs)) {
		if (ciblesActives.size() < maxCiblesSimultanees) {
			Target* nouvelleCible = randomiser->genererTarget();
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

void Jeu::verifierCollisions(const QRectF& rectangleReticule, qint64 tempsMs)
{
	if (enPause) {
		return;
	}

	for (Target* cible : ciblesActives) {
		if (cible && cible->estActif() && cible->intersecte(rectangleReticule)) {
			cible->jouerAnimationDestruction(CHEMIN_DESTRUCTION, COLONNES_DESTRUCTION, LIGNES_DESTRUCTION, CYCLE_DESTRUCTION);
			cible->detruire(tempsMs);
			score += cible->getPointsScore();
			ciblesTouchees++;
		}
	}
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

void Jeu::Tirer(const int x, const int y) {
	verifierCollisions(QRectF(x - 7, y - 7, 14, 14), 0);
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

void Jeu::initialiserCiblesParDefaut()
{
	DefinitionTarget buff;
	buff.cheminSprite = "/images/sprites/busch_ices.png";
	buff.colonnesSprite = 4;
	buff.lignesSprite = 3;
	buff.cycleAnimation = 800;
	buff.type = TypeTarget::BUFF;
	buff.taille = QSizeF(200, 200);
	buff.pointsScore = 10;
	buff.vitesseMin = 500.0;
	buff.vitesseMax = 1000.0;
	ajouterTypeCible(buff);

	DefinitionTarget debuff;
	debuff.cheminSprite = "/images/sprites/guiness.png";
	debuff.colonnesSprite = 4;
	debuff.lignesSprite = 3;
	debuff.cycleAnimation = 800;
	debuff.type = TypeTarget::DEBUFF;
	debuff.taille = QSizeF(250, 250);
	debuff.pointsScore = -15;
	debuff.vitesseMin = 500.0;
	debuff.vitesseMax = 1250.0;
	ajouterTypeCible(debuff);

	DefinitionTarget mixte;
	mixte.cheminSprite = "/images/sprites/redbull.png";
	mixte.colonnesSprite = 4;
	mixte.lignesSprite = 3;
	mixte.cycleAnimation = 800;
	mixte.type = TypeTarget::MIXTE;
	mixte.taille = QSizeF(200, 200);
	mixte.pointsScore = 20;
	mixte.vitesseMin = 500.0;
	mixte.vitesseMax = 1250.0;
	ajouterTypeCible(mixte);

	DefinitionTarget legendaire;
	legendaire.cheminSprite = "/images/sprites/golden_twisted_tea.png";
	legendaire.colonnesSprite = 4;
	legendaire.lignesSprite = 3;
	legendaire.cycleAnimation = 800;
	legendaire.type = TypeTarget::LEGENDAIRE;
	legendaire.taille = QSizeF(200, 200);
	legendaire.pointsScore = 50;
	legendaire.vitesseMin = 420.0;
	legendaire.vitesseMax = 20050.0;
	ajouterTypeCible(legendaire);

	DefinitionTarget bonus;
	bonus.cheminSprite = "/images/sprites/pabst_blue_ribbon.png";
	bonus.colonnesSprite = 4;
	bonus.lignesSprite = 3;
	bonus.cycleAnimation = 800;
	bonus.type = TypeTarget::BONUS;
	bonus.taille = QSizeF(200, 200);
	bonus.pointsScore = 30;
	bonus.vitesseMin = 500.0;
	bonus.vitesseMax = 1250.0;
	ajouterTypeCible(bonus);
}
