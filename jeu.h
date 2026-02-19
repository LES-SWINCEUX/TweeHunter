#ifndef JEU_H
#define JEU_H

#include "target.h"
#include "randomiser.h"
#include <QList>
#include <QSizeF>
#include <QPainter>

class Jeu
{
public:
	explicit Jeu(const QSizeF& tailleEcran);

	~Jeu();

	void update(qint64 tempsMs);

	void dessiner(QPainter& painter, qint64 tempsMs);

	void verifierCollisions(const QRectF& rectangleReticule, qint64 tempsMs);

	void reinitialiser();

	int getScore() const {
				return score;
	}
	int getNombreCiblesTouchees() const {
				return ciblesTouchees;
	}
	int getNombreCiblesManquees() const {
				return ciblesManquees;
	}
	int getNombreCiblesActives() const {
				return ciblesActives.size();
	}

	void setFrequenceSpawn(qint64 intervalMs);
	void setVariationFrequence(qint64 variationMs);
	void setMaxCiblesSimultanees(int max);
	void setTailleEcran(const QSizeF& taille);

	int getMaxCiblesSimultanees() const {
				return maxCiblesSimultanees;
	}
	void ajouterTypeCible(const DefinitionTarget& definition);

	void Tirer(const int x, const int y);

	void setPause(bool pause) {
				enPause = pause;
	}
	bool estEnPause() const {
						return enPause;
	}

private:
	
	void nettoyerCiblesInactives();

	void initialiserCiblesParDefaut();

	QList<Target*> ciblesActives;
	Randomiser* randomiser;

	int score;
	int ciblesTouchees;
	int ciblesManquees;
	int maxCiblesSimultanees;

	bool enPause;
};

#endif