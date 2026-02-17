#ifndef RANDOMISER_H
#define RANDOMISER_H

#include "target.h"
#include <qlist.h>
#include <QSizeF>
#include <random>

struct DefinitionTarget
{
	QString cheminSprite;
	int colonnesSprite = 4;
	int lignesSprite = 1;
	int cycleAnimation = 800;

	TypeTarget type = TypeTarget::BUFF;
	QSizeF taille = QSizeF(50, 50);
	int pointsScore = 10;
	double vitesseMin = 80.0;
	double vitesseMax = 150.0;
};

class Randomiser
{
public:
	explicit Randomiser(const QSizeF& tailleEcran);

	void ajouterTypeTarget(const DefinitionTarget& definition);

	bool doitGenererTarget(qint64 tempsMs);
	Target* genererTarget();

	void setFrequenceSpawn(qint64 intervalMs) {
		intervalSpawn = intervalMs;
	}
	void setVariationFrequence(qint64 variationMs) {
		variationSpawn = variationMs;
	}
	void setMarge(double marge) {
		margeEcran = marge;
	}
	void setTailleEcran(const QSizeF& taille) {
		tailleEcran = taille;	
	}
	qint64 getFrequenceSpawn() const {
		return intervalSpawn;
	}
	qint64 getVariationFrequence() const {
		return variationSpawn;
	}

private:
	
	QPointF choisirPointDepart(Bord bord) const;
	QPointF choisirPointArrivee(Bord bord) const;
	Bord choisirBordAleatoire() const;
	Bord choisirBordOppose(Bord bordDepart) const;
	double choisirVitesse(double min, double max) const;
	TypeTrajectoire choisirTrajectoire() const;

	QSizeF tailleEcran;
	QList<DefinitionTarget> typesDisponibles;

	qint64 prochainSpawn = 0;
	qint64 intervalSpawn;
	qint64 variationSpawn;
	double margeEcran;

	mutable std::mt19937 generateur;

	static constexpr qint64 INTERVALLE_SPAWN_DEFAUT = 1500;
	static constexpr qint64 VARIATION_SPAWN_DEFAUT = 500;
	static constexpr double MARGE_ECRAN_DEFAULT = 100.0;

};

#endif