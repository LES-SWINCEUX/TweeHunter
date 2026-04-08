#ifndef TOUCHES_H
#define TOUCHES_H

#include <SDL3/SDL.h>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <cmath>

#include "NativeSerialPort.h"
#include "configuration_partie.h"

class Touches : public QObject
{
	Q_OBJECT

public:
	Touches();
	~Touches();

	bool isJoystickConnected() const { return joystickOfficiel; }
	bool isJoystickPersoConnected() const { return joystickPerso; }
	bool isAnyConnected() const { return joystickOfficiel || joystickPerso; }
	SDL_Gamepad* getGamepad() const { return gamepad; }
	void verifierConnexion();
	// Met à jour l'état SDL et lit le port série — appeler une fois par tick
	void mettreAJour();
	bool RTpressed() const;
	bool LTpressed() const;

	// --- Lecture en jeu (émet les signaux d'action) ---
	void lireJeu(TypeManette manette, qint64 deltaMs);
	void lireNavigation();

	// --- Manette standard : navigation directionnelle ---
	bool haut() const;
	bool bas() const;
	bool gauche() const;
	bool droite() const;
	bool confirmer() const;
	bool retour() const;

	// --- Manette standard : axes analogiques normalisés [-1, 1] ---
	float axeX() const;
	float axeY() const;

	// --- Manette custom : navigation directionnelle ---
	bool customHaut() const;
	bool customBas() const;
	bool customGauche() const;
	bool customDroite() const;
	bool customConfirmer() const { return gachette; }
	bool customRetour()   const { return reload; }

	// --- Manette custom : axes analogiques normalisés [-1, 1] ---
	float customAxeX() const;
	float customAxeY() const;

	void lirePerso();
	int getxPerso() const;
	int getyPerso() const;

	bool getGachette() const { return gachette; }
	bool getReload() const { return reload; }
	bool getAccelerometre() const { return accelerometre; }
	int getEncodeur() const { return encodeur; }

	int useLastEncodeur();

    void envoyerNbBalles(int nbBalles);
	void envoyerMoteur();
    void envoyerRaw(const QByteArray& data);
	void envoyerFinPartie();

signals:
	void tireDemande();
	void reloadDemande();
	void pauseDemande();
	void powerUpDemande();
	void joystickDeplace(float deltaMs);

	// Navigation menu
	void naviguerHaut();
	void naviguerBas();
	void naviguerGauche();
	void naviguerDroite();
	void naviguerConfirmer();
	void naviguerRetour();

private:
	// Seuils internes
	static constexpr float kDeadZoneStandard = 0.5f;
	static constexpr float kDeadZoneCustom   = 0.35f;
	static constexpr int   kSeuilCustomHaut  = 300;
	static constexpr int   kSeuilCustomBas   = 700;
	static constexpr Sint16 kSeuilGachette   = 10000;

	// État précédent pour lireJeu()
	bool gachetteTirPrecedente     = false;
	bool gachettePowerUpPrecedente = false;
	bool reloadPrecedent           = false;
	bool startPrecedent            = false;
	bool powerUpActif              = false;

	// État précédent pour lireNavigation()
	bool navHautPrecedent         = false;
	bool navBasPrecedent          = false;
	bool navGauchePrecedent       = false;
	bool navDroitePrecedent       = false;
	bool navOkPrecedent           = false;
	bool navRetourPrecedent       = false;
	bool navVerrouJoystick        = false;
	bool navCustomHautPrecedent   = false;
	bool navCustomBasPrecedent    = false;
	bool navCustomGauchePrecedent = false;
	bool navCustomOkPrecedent     = false;
	bool navVerrouJoystickCustom  = false;

	bool joystickOfficiel = false;
	bool joystickPerso = false;

	SDL_Gamepad* gamepad = nullptr;
	int middleX = 0;
	int middleY = 0;

	int x = 500;
	int y = 500;
	bool gachette = false;
	bool reload = false;
	bool accelerometre = false;

	int  pendingX = 512;
	int  pendingY = 512;
	bool hasNewJoystick = false;
	int encodeur = 0;
	int lastEncodeur = 0;

	NativeSerialPort serial;
};

#endif
