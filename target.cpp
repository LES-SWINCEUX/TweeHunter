#include "target.h"
#include <QPainter>

Target::Target(const QString& cheminSprite, int colonnes, int lignes, int cycle, Mouvement* mouvement, TypeTarget type, const QSizeF& taille)
	: mouvement(mouvement), type(type), taille(taille), etat(EtatTarget::ACTIVE), pointsScore(0), m_dejaComptee(false), tempsDebutDestruction(0)
{
	chargerSprite(sprite, cheminSprite, colonnes, lignes, cycle);
	position = mouvement->getPositionDepart();
}

Target::~Target()
{
	delete mouvement;
}

void Target::chargerSprite(Sprite& sprite, const QString& cheminSprite, int colonnes, int lignes, int cycle)
{
	QString cheminResolu = cheminSprite.startsWith(":/")
		? cheminSprite
		: (QDir::currentPath() + cheminSprite);

	QSharedPointer<QPixmap> pix = SpriteManager::instance().getPixmap(cheminResolu);

	if (!pix || pix->isNull()) {
		std::cout << "TARGET::Erreur chargement Target ->" << cheminSprite.toStdString() << std::endl;
		return;
	}

	SpriteSheet sheet(pix, colonnes, lignes);
	sprite.setSprite(sheet);
	sprite.setCycle(cycle);
}

void Target::jouerAnimationDestruction(const QString& cheminSprite, int colonnes, int lignes, int cycle)
{
	chargerSprite(spriteDestruction, cheminSprite, colonnes, lignes, cycle);
	spriteDestruction.setClip(0, -1, false);
}

void Target::update(qint64 tempsMs)
{
	switch(etat)
	{
		case EtatTarget::ACTIVE:
		{
			position = mouvement->calculerPosition(tempsMs);

			if (mouvement->estArrive()) {
				etat = EtatTarget::INACTIVE;
			}
			break;
		}
		case EtatTarget::EN_DESTRUCTION:
		{
			if (tempsMs - tempsDebutDestruction >= DUREE_ANIMATION_DESTRUCTION) {
				etat = EtatTarget::INACTIVE;
			}
			break;
		}
		case EtatTarget::INACTIVE:
			break;
	}
}

void Target::dessiner(QPainter& painter, qint64 tempsMs)
{
	if (etat == EtatTarget::INACTIVE) {
		return;
	}

	QRect dest(static_cast<int>(position.x() - taille.width() / 2.0), static_cast<int>(position.y() - taille.height() / 2.0), static_cast<int>(taille.width()), static_cast<int>(taille.height()));

	if (etat == EtatTarget::EN_DESTRUCTION) {
		if (!spriteDestruction.estValide()) {
			return;
		}
		qint64 tempsLocal = tempsMs - tempsDebutDestruction;
		spriteDestruction.dessiner(painter, dest, tempsLocal, true, estMiroir);
	}
	else {
		if (!sprite.estValide()) {
			return;
		}
		sprite.dessiner(painter, dest, tempsMs, true, estMiroir);
	}
}

void Target::detruire(qint64 tempsMs)
{
	if (etat == EtatTarget::ACTIVE) {
		etat = EtatTarget::EN_DESTRUCTION;
		tempsDebutDestruction = tempsMs;
	}
}

bool Target::intersecte(const QPainterPath& cercleReticule) const
{
	if (etat != EtatTarget::ACTIVE)
	{
		return false;
	}

	int reductionHitbox = 30; // Réduction de la hitbox pour une meilleure jouabilité

	QPainterPath conversionQPainterPath;//Création d'un QPainterPath qui parmet de comparer le cercle du réticule avec le rectangle de la cible

	QRectF Hitbox = getBounds();
	Hitbox = Hitbox.adjusted(reductionHitbox, reductionHitbox, -reductionHitbox, -reductionHitbox); // Réduction de la hitbox pour une meilleure jouabilité
	
	conversionQPainterPath.addRect(Hitbox); // Ajout du rectangle de la cible au QPainterPath

	return conversionQPainterPath.intersects(cercleReticule);
}

QRectF Target::getBounds() const
{
	return QRectF(position.x() - taille.width() / 2.0, position.y() - taille.height() / 2.0, taille.width(), taille.height()
	);
}