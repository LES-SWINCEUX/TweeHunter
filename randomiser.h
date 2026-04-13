#ifndef RANDOMISER_H
#define RANDOMISER_H

#include "target.h"
#include <qlist.h>
#include <QSizeF>
#include <random>
#include <iostream>
#include "modejeu.h"
#include "targetbuff.h"
#include "targetdebuff.h"
#include "targetmixte.h"
#include "targetlegendaire.h"
#include "targetbonus.h"
#include "bush_louche.h"
#include "poison.h"
#include "water.h"
#include "gator.h"

struct DefinitionTarget
{
	TypeTarget type = TypeTarget::BUFF;
	double tailleRelative = 0.1;
	int pointsScore = 10;
	double vitesseMin = 80.0;
	double vitesseMax = 150.0;
	double frequenceSpawn = 1.0;
};

class Randomiser
{
public:
	explicit Randomiser(const QSizeF& tailleEcran);

	void ajouterTypeTarget(const DefinitionTarget& definition);

	bool doitGenererTarget(qint64 tempsMs);
	Target* genererTarget(ModeJeu mode);
	Target* genererTargetDeType(const DefinitionTarget& def, ModeJeu mode, qint64 tempsMs);

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
	bool genererBushLouche(qint64 tempsMs);
	TypeLouche choisirTypeBushLouche() const;
	int choisirIndexBush(int nombreBush) const;

	bool DemarrerWave(qint64 tempsMs);
	bool EnWave() const { 
		return enWave; 
	}

	void setFacteurVitesse(double facteur) {
		facteurVitesse = facteur;
	}

private:
	double calculerFacteurVitesse(qint64 tempsMs) const;
	double choisirVitesse(double min, double max) const;

	QPointF choisirPointDepart(Bord bord) const;
	QPointF choisirPointArrivee(Bord bord) const;
	Bord choisirBordOppose(Bord bordDepart) const;

	Bord choisirBordAleatoire() const;

	TypeTrajectoire choisirTrajectoire() const;

	QSizeF tailleEcran;
	QList<DefinitionTarget> typesDisponibles;

	qint64 prochainSpawn = 0;
	qint64 intervalSpawn;
	qint64 variationSpawn;
	double margeEcran;

	mutable std::mt19937 generateur;

	qint64 prochainBushLouche = 0;
	qint64 tempsDebutPartie = 0;
	qint64 tempsCourant = 0;

	const qint64 INTERVALLE_SPAWN_DEFAUT = 1500;
	const qint64 VARIATION_SPAWN_DEFAUT = 500;
	const double MARGE_ECRAN_DEFAULT = 100.0;

	const qint64 INTERVALLE_SPAWN_BUSH_LOUCHE = 11500;
	const qint64 VARIATION_SPAWN_BUSH_LOUCHE = 20000;

	const qint64 INTERVALLE_DIFFICULTE = 15000;
	const double MULTIPLICATEUR_PALIER = 1.15;
	const double FACTEUR_MAX = 3;
	bool enWave = false;
	qint64 prochaineWave = 0;
	static constexpr qint64 DUREE_WAVE = 10000;
	static constexpr qint64 VARIATION_WAVE = 15000;
	static constexpr qint64 VARIATION_DUREE_WAVE = 3000;
	static constexpr qint64 INTERVALLE_WAVE = 20000;

	double facteurVitesse = 1.0;
};

#endif