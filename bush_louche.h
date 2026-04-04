#ifndef BUSH_LOUCHE_H
#define BUSH_LOUCHE_H

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QPainter>
#include "sprite.h"
#include <QPainterPath>
#include "sprite_manager.h"
#include <QDir.h>
#include <QtMath>
#include "spritesheet.h"

enum class TypeLouche {
	LOUCHE_1,
	LOUCHE_2,
	BONUS_3
};

enum class EtatLouche {
	AVERTISSEMENT,
	APPARITION,
	ACTIF,
	DISPARITION,
	INACTIF,
};

class BushLouche
{
public:
	BushLouche(const QPointF& position, TypeLouche type, const QSizeF& tailleBase, const QString& cheminSprite, int colonnes, int lignes, int cycle,
		const QString& cheminAvertissement, int colonnesAvert, int lignesAvert, int cycleAvert);

	void update(qint64 tempsMs);
	void dessiner(QPainter& painter, qint64 tempsMs);
	void detruire();

	bool estActif() const {
		return etat == EtatLouche::ACTIF && !touchee;
	}

	bool estTirable() const {
		return (etat == EtatLouche::ACTIF || etat == EtatLouche::DISPARITION) && !touchee;
	}
	bool estInnactif() const {
		return etat == EtatLouche::INACTIF;
	}
	bool disparait() const {
		return disparait_;
	}
	void marquerSonJoue() {
		disparait_ = false;
	}

	TypeLouche getType() const {
		return type;
	}
	int getPointsScore() const;
	QString getChemin() const;
	QRectF getBounds() const;

	bool intersecte(const QPainterPath& cercleReticule) const;


private:
	QPointF position;
	QSizeF tailleBase;
	QSizeF tailleFinale;
	TypeLouche type;
	EtatLouche etat;
	Sprite sprite;
	Sprite spriteAvertissement;
	qint64 tempsCreation = 0;
	qint64 tempsEtatDebut = 0;
	bool touchee = false;
	bool disparait_ = false;

	static constexpr qint64 DUREE_APPARITION = 500;
	static constexpr qint64 DUREE_ACTIF = 750;
	static constexpr qint64 DUREE_AVERTISSEMENT = 1000;
};

#endif