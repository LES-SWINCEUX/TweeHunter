#ifndef JEU_H
#define JEU_H

#include "target.h"
#include "randomiser.h"
#include "compteur_points.h"
#include "compteur_balles.h"
#include "vie.h"
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
	static constexpr qint64 DUREE_MS = 1200;
};

struct Enpleineface {
	QPointF position;
	qint64 tempsDebut;
	QString cheminSprite;
	int niveau = 1;
	bool initialise = false;

	qint64 getDuree() const {
		return 2000 * niveau;
	}
	int getLargeur() const {
		return 200 * niveau;
	}

};

using namespace std;

class Jeu
{

public:
	Jeu(const QSizeF& tailleEcran, CompteurPoints* compteurPoints, CompteurBalles* compteurBalles, Vies* vies, ModeJeu mode = ModeJeu::PLUS_18, Armes* A = nullptr);

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
	bool PowerUp(const int x, const int y, qint64 tempsMs);

	bool Explosion(const int x, const int y, qint64 tempsMs, int explo);

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

private:

	static QSharedPointer<QPixmap> spriteDestruction;
	void nettoyerCiblesInactives();

	void initialiserCiblesParDefaut();

	QList<Target*> ciblesActives;
	Randomiser* randomiser;
	CompteurPoints* compteurPoints = nullptr;
	CompteurBalles* compteurBalles = nullptr;
	Vies* vies = nullptr;

	int score;
	int ciblesTouchees;
	int ciblesManquees;
	int maxCiblesSimultanees;

	ModeJeu modeActuel;
	bool enPause;

	QList<Bush*> bushes;
	BushLouche* bushLoucheActif = nullptr;
	GestionnaireAudio* gestionnaireAudio = nullptr;
	Armes* armes = nullptr;

	std::function<void()> onMoteurDemande;

	QList<IndicateurScore> indicateurs;
	void dessinerIndicateurs(QPainter& painter, qint64 tempsMs);
	void nettoyerIndicateurs(qint64 tempsMs);

	bool enWave = false;
	qint64 prochaineWave= 0;
	static constexpr qint64 DUREE_WAVE = 10000;
	static constexpr qint64 INTERVALLE_WAVE = 30000;
	void UpdateWave(qint64 tempsMs);

	QSizeF tailleEcran;
	int niveauDebuff = 0;
	QList<Enpleineface> enpleineface;
	void dessinerEnpleinefaces(QPainter& painter, qint64 tempsMs);
	void nettoyerEnpleinefaces(qint64 tempsMs);
};

#endif
