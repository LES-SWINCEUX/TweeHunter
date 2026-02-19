#include "mouvement.h"

Mouvement::Mouvement(const QPointF& debut, const QPointF& fin, double vitesse, TypeTrajectoire type)
	: positionDepart(debut), positionFin(fin), vitesse(vitesse), type(type), tempsDebut(0), arrive(false)
{
	QPointF delta = positionFin - positionDepart;
	distanceTotale = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());

	dureeTrajet = (distanceTotale / vitesse) * 1000.0;
}

QPointF Mouvement::calculerPosition(qint64 tempsEcouleMs)
{
	if (tempsDebut == 0) {
		tempsDebut = tempsEcouleMs;
	}
	qint64 deltaTemps = tempsEcouleMs - tempsDebut;
	
	double progression = qMin(1.0, deltaTemps / dureeTrajet);


	if (progression >= 1.0) {
		arrive = true;
		return positionFin;
	}

	switch (type)
	{
	case TypeTrajectoire::LINEAIRE:
		return calculerPositionLineaire(progression);
	case TypeTrajectoire::COURBE_HAUT:
		return calculerPositionCourbe(progression, 1.0);
	case TypeTrajectoire::COURBE_BAS:
		return calculerPositionCourbe(progression, -1.0);
	case TypeTrajectoire::ZIGZAG:
		return calculerPositionZigzag(progression);
	default:
		return calculerPositionLineaire(progression);
	}
}

bool Mouvement::estArrive() const
{
	return arrive;
}

void Mouvement::reinitialiser()
{
	tempsDebut = 0;
	arrive = false;
}

QPointF Mouvement::calculerPositionLineaire(double progression)
{
	return positionDepart + (positionFin - positionDepart) * progression;
}

QPointF Mouvement::calculerPositionCourbe(double progression, double direction)
{
	QPointF posBase = calculerPositionLineaire(progression);
	double offset = qSin(progression * M_PI) * (distanceTotale * 0.1) * direction;

	QPointF delta = positionFin - positionDepart;
	QPointF perpendiculaire(-delta.y(), delta.x());

	double longueur = qSqrt(perpendiculaire.x() * perpendiculaire.x() + perpendiculaire.y() * perpendiculaire.y());

	if (longueur > 0.0) {
		perpendiculaire /= longueur;
		perpendiculaire *= offset;
	}
	return posBase + perpendiculaire;
}

QPointF Mouvement::calculerPositionZigzag(double progression)
{
	QPointF posBase = calculerPositionLineaire(progression);

	double offset = qSin(progression * FREQUENCE_ZIGZAG * 2.0 * M_PI) * AMPLITUDE_ZIGZAG;

	QPointF delta = positionFin - positionDepart;
	QPointF perpendiculaire(-delta.y(), delta.x());
	double longueur = qSqrt(perpendiculaire.x() * perpendiculaire.x() + perpendiculaire.y() * perpendiculaire.y());

	if (longueur > 0.0) {
		perpendiculaire /= longueur;
		perpendiculaire *= offset;
	}
	return posBase + perpendiculaire;
}