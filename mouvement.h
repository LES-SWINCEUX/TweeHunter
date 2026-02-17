#ifndef MOUVEMENT_H
#define MOUVEMENT_H

#include <QPointF>
#include <QRectF>
#include <QtMath>

enum class TypeTrajectoire
{
	LINEAIRE,
	COURBE_HAUT,
	COURBE_BAS,
	ZIGZAG
};

enum class Bord
{
	HAUT,
	BAS,
	GAUCHE,
	DROITE
};

class Mouvement
{
public:

	Mouvement(const QPointF& debut, const QPointF& fin, double vitesse, TypeTrajectoire type = TypeTrajectoire::LINEAIRE);

	QPointF calculerPosition(qint64 tempsEcouleMs);

	bool estArrive() const;

	void reinitialiser();

	QPointF getPositionDepart() const {
		return positionDepart;
	}
	QPointF getPositionFin() const {
		return positionFin;
	}
	double getVitesse() const {
		return vitesse;
	}
	TypeTrajectoire getType() const {
		return type;
	}

	void setVitesse(double v) {
		vitesse = v;
	}
	void setType(TypeTrajectoire t) {
		type = t;
	}

private:
	
	QPointF calculerPositionLineaire(double progression);
	QPointF calculerPositionCourbe(double progression, double direction);
	QPointF calculerPositionZigzag(double progression);

	QPointF positionDepart;
	QPointF positionFin;
	double vitesse; // pixels par seconde
	TypeTrajectoire type;

	qint64 tempsDebut;
	double distanceTotale;
	double dureeTrajet; //ms
	bool arrive;

	static constexpr double AMPLITUDE_ZIGZAG = 50.0;
	static constexpr double FREQUENCE_ZIGZAG = 5.0;

};

#endif