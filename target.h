#ifndef Target_H
#define Target_H

#include "sprite.h"
#include "sprite_manager.h"
#include "spritesheet.h"
#include "mouvement.h"
#include <QRectF>
#include <QPointF>
#include <QDir>
#include <QSharedPointer>
#include <iostream>


enum class TypeTarget
{
	BUFF,
	DEBUFF,
	MIXTE,
	LEGENDAIRE,
	BONUS
};

enum class EtatTarget
{
	ACTIVE,
	EN_DESTRUCTION,
	INACTIVE
	
};

class Target
{
public:
	Target(const QString& cheminSprite, int colonnes, int lignes, int cycle, Mouvement* mouvement, TypeTarget type = TypeTarget::BUFF, const QSizeF& taille = QSizeF(50, 50));

	~Target();

	void update(qint64 tempsMs);
	void dessiner(QPainter& painter, qint64 tempsMs);
	void detruire(qint64 tempsMs);
	void jouerAnimationDestruction(const QString& cheminSprite, int colonnes, int lignes, int cycle);

	bool intersecte(const QRectF& rectangleReticule) const;
	
	bool estActif() const {
		return etat == EtatTarget::ACTIVE;
	}
	bool estInactif() const {
		return etat == EtatTarget::INACTIVE;
	}
	bool estEnDestruction() const {
				return etat == EtatTarget::EN_DESTRUCTION;
	}
	EtatTarget getEtat() const {
				return etat;
	}
	TypeTarget getType() const {
				return type;
	}
	QPointF getPosition() const {
				return position;
	}
	QRectF getBounds() const;
	int getPointsScore() const {
				return pointsScore;
	}
	bool dejaComptee() const {
				return m_dejaComptee;
	}

	void setPointsScore(int points) {
				pointsScore = points;
	}

	void marquerComptee() {
				m_dejaComptee = true;
	}

	void setMiroir(bool miroir) {
				estMiroir = miroir;
	}

private:
	void chargerSprite(Sprite& sprite, const QString& cheminSprite, int colonnes, int lignes, int cycle);

	Sprite sprite;
	Sprite spriteDestruction;
	Mouvement* mouvement;

	QPointF position;
	QSizeF taille; 
	TypeTarget type;
	EtatTarget etat;
	int pointsScore;
	bool m_dejaComptee;

	bool estMiroir = false;
	qint64 tempsDebutDestruction;

	static constexpr qint64 DUREE_ANIMATION_DESTRUCTION = 500;
};

#endif
