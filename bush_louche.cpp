#include "bush_louche.h"

BushLouche::BushLouche(const QPointF& position, TypeLouche type, const QSizeF& tailleBase, const QString& cheminSprite, int colonnes, int lignes, int cycle,
	const QString& cheminAvertissement, int colonnesAvert, int lignesAvert, int cycleAvert)
	: position(position), type(type), tailleBase(tailleBase), etat(EtatLouche::AVERTISSEMENT), tempsCreation(0), tempsEtatDebut(0), touchee(false)
{
	tailleFinale = QSizeF(tailleBase.width() * 1.0, tailleBase.height() * 1.0);

	QString cheminResolu = QDir::currentPath() + cheminSprite;
	QSharedPointer<QPixmap> pix = SpriteManager::instance().getPixmap(cheminResolu);
	if (pix && !pix->isNull()) {
		SpriteSheet sheet(pix, colonnes, lignes);
		sprite.setSprite(sheet);
		sprite.setCycle(cycle);

	}
	QString cheminAvert = QDir::currentPath() + "/images/Bush/avert.png";
	QSharedPointer<QPixmap> pixAvert = SpriteManager::instance().getPixmap(cheminAvert);
	if (pixAvert && !pixAvert->isNull()) {
		SpriteSheet sheetAvert(pixAvert, colonnesAvert, lignesAvert);
		spriteAvertissement.setSprite(sheetAvert);
		spriteAvertissement.setCycle(cycleAvert);
	}
}

void BushLouche::update(qint64 tempsMs)
{
	if (tempsCreation == 0) {
		tempsCreation = tempsMs;
		tempsEtatDebut = tempsMs;

		qint64 dureeVisible = DUREE_APPARITION + DUREE_ACTIF + DUREE_APPARITION;
		sprite.setCycle(static_cast<int>(dureeVisible));
		sprite.setClip(0, -1, false);
	}

	qint64 delta = tempsMs - tempsEtatDebut;

	switch (etat) {
	case EtatLouche::AVERTISSEMENT:
		if (delta >= DUREE_AVERTISSEMENT) {
			etat = EtatLouche::APPARITION;
			tempsEtatDebut = tempsMs;
		}
		break;

	case EtatLouche::APPARITION:
		if (delta >= DUREE_APPARITION) {
			etat = EtatLouche::ACTIF;
			tempsEtatDebut = tempsMs;
		}
		break;

	case EtatLouche::ACTIF:
		if (delta >= DUREE_ACTIF) {
			etat = EtatLouche::DISPARITION;
			tempsEtatDebut = tempsMs;
			disparait_ = true;
		}
		break;

	case EtatLouche::DISPARITION:
		if (delta >= DUREE_APPARITION) {
			etat = EtatLouche::INACTIF;
		}
		break;

	case EtatLouche::INACTIF:
		break;
	}
}

void BushLouche::dessiner(QPainter& painter, qint64 tempsMs)
{
	if (etat == EtatLouche::INACTIF) {
		return;
	}

	qint64 tempsLocal = tempsMs - tempsEtatDebut;

	//Avertissement

	if (etat == EtatLouche::AVERTISSEMENT) {
		if (!spriteAvertissement.estValide()) {
			return;
		}

		double offsetY = tailleBase.height() * 0.2;

		QRect dest(static_cast<int>(position.x() - tailleBase.width() / 2),
			static_cast<int>(position.y() - tailleBase.height() / 2) + offsetY,
			static_cast<int>(tailleBase.width()), static_cast<int>(tailleBase.height()));

		spriteAvertissement.dessiner(painter, dest, tempsLocal, true, false);
		return;
	}
	if (!sprite.estValide()) {
		return;
	}

	//taille selon etat (plus utilise) 

	QSizeF tailleCourante = tailleFinale;

	if (etat == EtatLouche::APPARITION) {
		double progression = qMin(1.0, tempsLocal / (double)DUREE_APPARITION);
		tailleCourante = QSizeF(
			tailleBase.width() + (tailleFinale.width() - tailleBase.width()) * progression,
			tailleBase.height() + (tailleFinale.height() - tailleBase.height()) * progression
		);
	}

	else if (etat == EtatLouche::DISPARITION) {
		double progression = qMin(1.0, tempsLocal / (double)DUREE_APPARITION);
		tailleCourante = QSizeF(
			tailleFinale.width() - (tailleFinale.width() - tailleBase.width()) * progression,
			tailleFinale.height() - (tailleFinale.height() - tailleBase.height()) * progression
		);
	}
	//Animation

	qint64 tempsAnimation;
	if (etat == EtatLouche::DISPARITION) {
		qint64 dureeVisible = DUREE_APPARITION + DUREE_ACTIF + DUREE_APPARITION;
		qint64 tempsForward = tempsMs - tempsCreation - DUREE_AVERTISSEMENT;
		tempsAnimation = dureeVisible - (tempsForward - (DUREE_APPARITION + DUREE_ACTIF));
		if (tempsAnimation < 0) tempsAnimation = 0;
	}
	else {
		tempsAnimation = tempsMs - tempsCreation - DUREE_AVERTISSEMENT;
		if (tempsAnimation < 0) tempsAnimation = 0;
	}

	QRect dest(static_cast<int>(position.x() - tailleCourante.width() / 2),
		static_cast<int>(position.y() - tailleCourante.height() / 2), static_cast<int>(tailleCourante.width()), static_cast<int>(tailleCourante.height()));
	sprite.dessiner(painter, dest, tempsAnimation, true, false);
}

void BushLouche::detruire()
{
	touchee = true;
}

QRectF BushLouche::getBounds() const
{
	return QRectF(position.x() - tailleFinale.width() / 2, position.y() - tailleFinale.height() / 2, tailleFinale.width(), tailleFinale.height());
}

int BushLouche::getPointsScore() const
{
	switch (type) {
	case TypeLouche::LOUCHE_1:
		return -300;
	case TypeLouche::LOUCHE_2:
		return -300;
	case TypeLouche::BONUS_3:
		return 600;
	}

	return 0;
}

QString BushLouche::getChemin() const
{
	switch (type) {
	case TypeLouche::LOUCHE_1:
		return "/images/Bush/Domingoat.png";
	case TypeLouche::LOUCHE_2:
		return "/images/Bush/JP.png";
	case TypeLouche::BONUS_3:
		return "/images/Bush/busch.png";
	}
	return "";
}

bool BushLouche::intersecte(const QPainterPath& cercleReticule) const
{
	if (!estTirable()) {
		return false;
	}

	int reductionHitbox = 30;
	QRectF Hitbox = getBounds();
	Hitbox = Hitbox.adjusted(reductionHitbox, reductionHitbox, -reductionHitbox, -reductionHitbox);

	return cercleReticule.intersects(Hitbox);
}