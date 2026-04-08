#include "jeu.h"

QSharedPointer<QPixmap> Jeu::spriteDestruction = nullptr;

Jeu::Jeu(const QSizeF& tailleEcran, CompteurPoints* compteurPoints, CompteurBalles* compteurBalles, CompteurVies* compteurVies, ModeJeu mode, Armes* A, CompteurPowerUp* C)
	: randomiser(nullptr), score(0), ciblesTouchees(0), ciblesManquees(0), maxCiblesSimultanees(4), enPause(false), modeActuel(mode), tailleEcran(tailleEcran)
{
	armes = A;
	compteurPowerUp = C;

	if (!spriteDestruction) {
		QString chemin = QDir::currentPath() + "/images/sprites/Explosion.png";
		spriteDestruction = SpriteManager::instance().getPixmap(chemin);
	}
	randomiser = new Randomiser(tailleEcran);

	double tailleBush = tailleEcran.width() * 0.15;
	bushes.append(new Bush(
		QPointF(tailleEcran.width() * 0.25, tailleEcran.height() * 0.67),
		QSizeF(tailleBush, tailleBush),
		"/images/Bush/Domingoat.png"
	));
	bushes.append(new Bush(
		QPointF(tailleEcran.width() * 0.5, tailleEcran.height() * 0.67),
		QSizeF(tailleBush, tailleBush),
		"/images/Bush/JP.png"
	));
	bushes.append(new Bush(
		QPointF(tailleEcran.width() * 0.75, tailleEcran.height() * 0.67),
		QSizeF(tailleBush, tailleBush),
		"/images/Bush/busch.png"
	));

	if (gestionnaireAudio) {
		gestionnaireAudio->addSfx("louche_1", QDir::currentPath() + "/sounds/louche1.wav", 2);
		gestionnaireAudio->addSfx("louche_2", QDir::currentPath() + "/sounds/louche2.wav", 2);
		gestionnaireAudio->addSfx("bonus_3", QDir::currentPath() + "/sounds/bonus3.wav", 2);
		gestionnaireAudio->addSfx("disparait", QDir::currentPath() + "/sounds/destruction.wav", 4);
	}


	randomiser->setFrequenceSpawn(1000);
	randomiser->setVariationFrequence(500);
	randomiser->setMarge(20.0);

	this->compteurPoints = compteurPoints;
	this->compteurBalles = compteurBalles;
	this->compteurVies = compteurVies;

	initialiserCiblesParDefaut();
	qDebug() << QDir::currentPath();
}

Jeu::~Jeu()
{
	qDeleteAll(ciblesActives);
	ciblesActives.clear();
	delete randomiser;

	qDeleteAll(bushes);
	bushes.clear();
	if (bushLoucheActif) {
		delete bushLoucheActif;
	}
}

void Jeu::update(qint64 tempsMs)
{
	if (enPause) {
		return;

	}

	UpdateWave(tempsMs);

	for (auto& epf : enpleineface) {
		if (!epf.initialise) {
			epf.tempsDebut = tempsMs;
			epf.initialise = true;
		}
	}

	nettoyerEnpleinefaces(tempsMs);

	if (randomiser && randomiser->doitGenererTarget(tempsMs)) {
		if ((int)ciblesActives.size() < maxCiblesSimultanees) {
			Target* nouvelleCible = randomiser->genererTarget(modeActuel);
			if (nouvelleCible) {

				if (nouvelleCible->getType() == TypeTarget::DEBUFF) {
					nouvelleCible->setCallbackQuandTouchee([this](QPointF) {
						niveauDebuff++;
						enpleineface.clear();

						Enpleineface epf;
						epf.position = QPointF(tailleEcran.width() / 2, tailleEcran.height() / 2);
						epf.cheminSprite = "/images/sprites/splash.png";
						epf.niveau = niveauDebuff;
						epf.initialise = false;
						enpleineface.append(epf);
						});
				}

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
				if (cible->getType() == TypeTarget::WATER && !cible->aEteDetruite()) {
					compteurVies->setDemiVies(compteurVies->getDemiVies() - 2);
				}
			}
		}

	}

	if (!bushLoucheActif && randomiser->genererBushLouche(tempsMs)) {
		int indexBush = randomiser->choisirIndexBush(bushes.size());
		TypeLouche typeLouche = randomiser->choisirTypeBushLouche();

		QString cheminSprite;
		int colonnes = 4, lignes = 1, cycle = 1000;

		switch (typeLouche) {
		case TypeLouche::LOUCHE_1:
			cheminSprite = "/images/Bush/Domingoat.png";
			break;
		case TypeLouche::LOUCHE_2:
			cheminSprite = "/images/Bush/JP.png";
			break;
		case TypeLouche::BONUS_3:
			cheminSprite = "/images/Bush/busch.png";
			break;
		}

		bushLoucheActif = new BushLouche(
			bushes[indexBush]->getPosition(),
			typeLouche,
			bushes[indexBush]->getTaille(),
			cheminSprite, colonnes, lignes, cycle,
			"/images/sprites/Avertissement.png", 4, 1, 1000
		);
	}

	if (bushLoucheActif) {
		bushLoucheActif->update(tempsMs);

		if (bushLoucheActif->disparait()) {
			if (gestionnaireAudio) {
				gestionnaireAudio->playSfx("disparait");
			}
			bushLoucheActif->marquerSonJoue();
		}
		if (bushLoucheActif->estInnactif()) {
			delete bushLoucheActif;
			bushLoucheActif = nullptr;
		}
	}


	nettoyerCiblesInactives();
	nettoyerIndicateurs(tempsMs);
}

void Jeu::dessiner(QPainter& painter, qint64 tempsMs)
{
	for (Bush* bush : bushes) {
		if (bush) {
			bush->dessiner(painter);
		}
	}

	if (bushLoucheActif) {
		bushLoucheActif->dessiner(painter, tempsMs);
	}

	for (Target* cible : ciblesActives) {
		if (cible) {
			cible->dessiner(painter, tempsMs);
		}
	}

	dessinerIndicateurs(painter, tempsMs);
	dessinerEnpleinefaces(painter, tempsMs);
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

			IndicateurScore indic;
			indic.position = cible->getBounds().center();
			indic.points = incrementScores;
			indic.tempsDebut = tempsMs;
			indicateurs.append(indic);

			if (score <= 0) {
				score = 0;
			}

			if (incrementScores < 0) {
				compteurVies->setDemiVies(compteurVies->getDemiVies() - 1);
			}

			// Comportements spéciaux par type
			if (cible->getType() == TypeTarget::POISON) {
				compteurVies->setDemiVies(compteurVies->getDemiVies() - 1);
			}
			if (cible->getType() == TypeTarget::GATOR) {
				compteurVies->setDemiVies(compteurVies->getDemiVies() + 1);
			}
			if (cible->getType() ==TypeTarget::BONUS) {
				Explosion(cible->getPosition().x(), cible->getPosition().y(), tempsMs);

			}
			if(cible->getType() == TypeTarget::LEGENDAIRE) {
				compteurPowerUp->setPowerUp(compteurPowerUp->getPowerUp() + armes->addPowerUp());
			}

			compteurPoints->setPoints(score);
			ciblesTouchees++;

			if (cible->getType() == TypeTarget::MIXTE) {
				if (onMoteurDemande) {
					onMoteurDemande();
				}
			}
		}
	}

	if (bushLoucheActif && bushLoucheActif->estTirable() && bushLoucheActif->intersecte(cercleReticule)) {
		bushLoucheActif->detruire();

		int pointsLouche = bushLoucheActif->getPointsScore();
		score += pointsLouche;

		IndicateurScore indicBush;
		indicBush.position = bushLoucheActif->getBounds().center();
		indicBush.points = pointsLouche;
		indicBush.tempsDebut = tempsMs;
		indicateurs.append(indicBush);

		if (score <= 0) {
			score = 0;
		}

		if (pointsLouche < 0) {
			compteurVies->setDemiVies(compteurVies->getDemiVies() - 2);
		}

		compteurPoints->setPoints(score);
	}
	return aTouche;

		//if (gestionnaireAudio) {
		//	switch (bushLoucheActif->getType()) {
		//	case TypeLouche::LOUCHE_1:
		//		gestionnaireAudio->playSfx("louche_1");
		//		break;
		//	case TypeLouche::LOUCHE_2:
		//		gestionnaireAudio->playSfx("louche_2");
		//		break;
		//	case TypeLouche::BONUS_3:
		//		gestionnaireAudio->playSfx("bonus_3");
		//		break;
		//	}
		//}

}


void Jeu::reinitialiser()
{
	qDeleteAll(ciblesActives);
	ciblesActives.clear();

	score = 0;
	ciblesTouchees = 0;
	ciblesManquees = 0;
	enPause = false;

	if (bushLoucheActif) {
		delete bushLoucheActif;
		bushLoucheActif = nullptr;

	}
}

bool Jeu::Tirer(const int x, const int y, qint64 tempsMs) {

	if (compteurBalles && compteurBalles->getBalles() <= 0) {
		return false;
	}
	if (compteurBalles) {
		compteurBalles->setBalles(compteurBalles->getBalles() - 1);
	}
	return verifierCollisions(armes->choixArme(x, y), tempsMs);
}

bool Jeu::TireGratuit(const int x, const int y, qint64 tempsMs) {
	return verifierCollisions(armes->choixArme(x, y), tempsMs);
}

bool Jeu::PowerUp(const int x, const int y, PowerUpType choix, qint64 tempsMs) {
	return verifierCollisions(armes->Hitbox(choix, x, y), tempsMs);
}

bool Jeu::Explosion(const int x, const int y, qint64 tempsMs) {
	return verifierCollisions(armes->Hitbox(PowerUpType::GRENADE, x, y), tempsMs);
}

void Jeu::nettoyerCiblesInactives()
{
	auto it = ciblesActives.begin();
	while (it != ciblesActives.end()) {
		if ((*it)->estInactif()) {
			delete* it;
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
	debuff.pointsScore = 15;
	debuff.vitesseMin = 500.0;
	debuff.vitesseMax = 1250.0;
	debuff.frequenceSpawn = 2;
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
	legendaire.frequenceSpawn = 5.0;
	ajouterTypeCible(legendaire);

	DefinitionTarget bonus;
	bonus.type = TypeTarget::BONUS;
	bonus.tailleRelative = 0.15;
	bonus.pointsScore = 30;
	bonus.vitesseMin = 500.0;
	bonus.vitesseMax = 1250.0;
	bonus.frequenceSpawn = 6.0;
	ajouterTypeCible(bonus);

	DefinitionTarget poison;
	poison.type = TypeTarget::POISON;
	poison.tailleRelative = 0.15;
	poison.pointsScore = 0;
	poison.vitesseMin = 500.0;
	poison.vitesseMax = 1250.0;
	poison.frequenceSpawn = 10.0;
	ajouterTypeCible(poison);

	DefinitionTarget water;
	water.type = TypeTarget::WATER;
	water.tailleRelative = 0.15;
	water.pointsScore = 0;
	water.vitesseMin = 500.0;
	water.vitesseMax = 1000.0;
	water.frequenceSpawn = 10.0;
	ajouterTypeCible(water);

	DefinitionTarget gator;
	gator.type = TypeTarget::GATOR;
	gator.tailleRelative = 0.15;
	gator.pointsScore = 0;
	gator.vitesseMin = 1000.0;
	gator.vitesseMax = 1900.0;
	gator.frequenceSpawn = 10.0;
	ajouterTypeCible(gator);
}

void Jeu::nettoyerIndicateurs(qint64 tempsMs)
{
	auto it = indicateurs.begin();
	while (it != indicateurs.end()) {
		if (tempsMs - it->tempsDebut >= IndicateurScore::DUREE_MS) {
			it = indicateurs.erase(it);
		}
		else {
			++it;
		}
	}
}

void Jeu::dessinerIndicateurs(QPainter& painter, qint64 tempsMs)
{
	if (indicateurs.isEmpty()) return;

	static QFont font = []() {
		QFont f;
		f.setFamily("Press Start 2P");
		f.setPixelSize(26);
		f.setBold(true);
		f.setStyleStrategy(QFont::NoAntialias);
		return f;
	}();
	painter.setFont(font);

	for (const IndicateurScore& indic : indicateurs) {
		qint64 tempsEcoule = tempsMs - indic.tempsDebut;
		if (tempsEcoule < 0 || tempsEcoule >= IndicateurScore::DUREE_MS) continue;

		double t = double(tempsEcoule) / double(IndicateurScore::DUREE_MS);

		double decalageY = -60.0 * t;

		int alpha = (t < 0.6) ? 255 : int(255.0 * (1.0 - (t - 0.6) / 0.4));
		alpha = qBound(0, alpha, 255);

		QColor couleurTexte = (indic.points >= 0) ? QColor(80, 255, 80, alpha) : QColor(255, 80, 80, alpha);

		QString texte = (indic.points >= 0) ? QString("+%1").arg(indic.points) : QString("%1").arg(indic.points);

		QPointF pos = indic.position + QPointF(0, decalageY);

		QColor ombre(0, 0, 0, alpha / 2);
		painter.setPen(ombre);
		painter.drawText(QRectF(pos.x() - 60 + 2, pos.y() - 20 + 2, 120, 40), Qt::AlignCenter, texte);

		painter.setPen(couleurTexte);
		painter.drawText(QRectF(pos.x() - 60, pos.y() - 20, 120, 40), Qt::AlignCenter, texte);
	}
}

void Jeu::UpdateWave(qint64 tempsMs)
{
	if (randomiser->DemarrerWave(tempsMs)) {
		maxCiblesSimultanees = 10;
		randomiser->setFrequenceSpawn(100);
	}
	else if (!randomiser->EnWave() && maxCiblesSimultanees == 10) {
		maxCiblesSimultanees = 5;
		randomiser->setFrequenceSpawn(1000);
	}
}

void Jeu::nettoyerEnpleinefaces(qint64 tempsMs)
{
	auto it = enpleineface.begin();
	while (it != enpleineface.end()) {
		if (it->initialise && tempsMs - it->tempsDebut >= it->getDuree()) {
			it = enpleineface.erase(it);
			niveauDebuff = 0;
		}
		else {
			++it;
		}
	}
}
void Jeu::dessinerEnpleinefaces(QPainter& painter, qint64 tempsMs)
{
	if (enpleineface.isEmpty()) return;

	for (const Enpleineface& epf : enpleineface) {
		if (!epf.initialise) continue;

		qint64 tempsEcoule = tempsMs - epf.tempsDebut;
		if (tempsEcoule < 0 || tempsEcoule >= epf.getDuree()) continue;

		double t = double(tempsEcoule) / double(epf.getDuree());
		int alpha = (t < 0.90) ? 255 : int(255.0 * (1.0 - (t - 0.90) / 0.10));
		alpha = qBound(0, alpha, 255);

		QSharedPointer<QPixmap> pix = SpriteManager::instance().getPixmap(
			QDir::currentPath() + epf.cheminSprite
		);
		if (!pix || pix->isNull()) continue;

		int largeur = epf.getLargeur(tailleEcran.width());
		QRect dest(
			static_cast<int>(epf.position.x() - largeur / 2),
			static_cast<int>(epf.position.y() - largeur / 2),
			largeur, largeur
		);

		painter.save();
		painter.setOpacity(alpha / 255.0);
		painter.drawPixmap(dest, *pix);
		painter.restore();
	}
}