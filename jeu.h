#ifndef JEU_H
#define JEU_H

#include "target.h"
#include "randomiser.h"
#include "compteur_points.h"
#include "compteur_balles.h"
#include "compteur_vies.h"
#include "compteur_powerup.h"
#include <QList>
#include <QSizeF>
#include <QTimer>
#include <QPainter>
#include <iostream>
#include "modejeu.h"
#include <QPainterPath>
#include "bush.h"
#include "bush_louche.h"
#include "gestionnaire_audio.h"
#include "Armes.h"
#include <functional>
#include <QFont>
#include <QColor>

struct IndicateurScore {
	QPointF position;
	int points;
	qint64  tempsDebut;
	static const qint64 DUREE_MS = 1200;
};

struct Enpleineface {
	QPointF position;
	qint64 tempsDebut;
	QString cheminSprite;
	int niveau = 1;
	bool initialise = false;

	qint64 getDuree() const {
		return (1000 * niveau);
	}
	int getLargeur(double largeurEcran) const {
		return static_cast<int>(largeurEcran * 0.15 * niveau);
	}

};

class Jeu
{

public:
	Jeu(const QSizeF& tailleEcran, CompteurPoints* compteurPoints, CompteurBalles* compteurBalles, CompteurVies* vies, ModeJeu mode = ModeJeu::PLUS_18, Armes* A = nullptr, CompteurPowerUp* C=nullptr);

	~Jeu();

	void update(qint64 tempsMs);

	void dessiner(QPainter& painter, qint64 tempsMs);

	bool verifierCollisions(const QPainterPath& cercleReticule, qint64 tempsMs);

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

	bool Tirer(const int x, const int y, qint64 tempsMs);
	bool TireGratuit(const int x, const int y, qint64 tempsMs);
	bool PowerUp(const int x, const int y, PowerUpType choix, qint64 tempsMs);

	bool Explosion(const int x, const int y, qint64 tempsMs);

	Armes* getArmes() const { return this->armes; }

	void setModeJeu(ModeJeu mode);
	ModeJeu getModeJeu() const {
		return modeActuel;
	}

	void setPause(bool pause) {
		enPause = pause;
	}
	bool estEnPause() const {
		return enPause;
	}
	void setOnMoteurDemande(std::function<void()> callback) {
		onMoteurDemande = callback;
	}

	void setFacteurVitesse(double facteur) {
		if (randomiser) randomiser->setFacteurVitesse(facteur);
	}

private:
	void nettoyerCiblesInactives();
	void initialiserCiblesParDefaut();

	void dessinerEnpleinefaces(QPainter& painter, qint64 tempsMs);
	void dessinerIndicateurs(QPainter& painter, qint64 tempsMs);

	void nettoyerEnpleinefaces(qint64 tempsMs);
	void nettoyerIndicateurs(qint64 tempsMs);
	void UpdateWave(qint64 tempsMs);

	static QSharedPointer<QPixmap> spriteDestruction;

	QList<Target*> ciblesActives;
	Randomiser* randomiser;
	CompteurPoints* compteurPoints = nullptr;
	CompteurBalles* compteurBalles = nullptr;
	CompteurVies* compteurVies = nullptr;

	int score;
	int ciblesTouchees;
	int ciblesManquees;
	int maxCiblesSimultanees;
	int niveauDebuff = 0;

	bool enPause;
	bool enWave = false;

	ModeJeu modeActuel;

	QList<Bush*> bushes;
	BushLouche* bushLoucheActif = nullptr;
	GestionnaireAudio* gestionnaireAudio = nullptr;
	Armes* armes = nullptr;
	CompteurPowerUp* compteurPowerUp = nullptr;

	std::function<void()> onMoteurDemande;

	QList<IndicateurScore> indicateurs;

	void UpdateWave(qint64 tempsMs);

	QSizeF tailleEcran;
	QList<Enpleineface> enpleineface;

	const int COLONNES_DESTRUCTION = 4;
	const int LIGNES_DESTRUCTION = 3;
	const int CYCLE_DESTRUCTION = 1000;

	const qint64 DUREE_WAVE = 10000;
	const qint64 INTERVALLE_WAVE = 30000;

	const QString CHEMIN_DESTRUCTION = "/images/sprites/Explosion.png";
};

#endif
