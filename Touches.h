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

	void mettreAJour();
	bool RTpressed() const;
	bool LTpressed() const;

	void lireJeu(TypeManette manette, qint64 deltaMs);
	void lireNavigation();

	bool haut() const;
	bool bas() const;
	bool gauche() const;
	bool droite() const;
	bool confirmer() const;
	bool retour() const;

	float axeX() const;
	float axeY() const;

	bool customHaut() const;
	bool customBas() const;
	bool customGauche() const;
	bool customDroite() const;
	bool customConfirmer() const { return gachette; }
	bool customRetour()   const { return reload; }

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
	void joystickDeplace(float dx, float dy, float deltaMs);

	void naviguerHaut();
	void naviguerBas();
	void naviguerGauche();
	void naviguerDroite();
	void naviguerConfirmer();
	void naviguerRetour();

private:
	bool gachetteTirPrecedente = false;
	bool gachettePowerUpPrecedente = false;
	bool reloadPrecedent = false;
	bool startPrecedent = false;
	bool powerUpActif = false;

	bool navHautPrecedent = false;
	bool navBasPrecedent = false;
	bool navGauchePrecedent = false;
	bool navDroitePrecedent = false;
	bool navOkPrecedent = false;
	bool navRetourPrecedent = false;
	bool navVerrouJoystick = false;
	bool navCustomHautPrecedent = false;
	bool navCustomBasPrecedent = false;
	bool navCustomGauchePrecedent = false;
	bool navCustomOkPrecedent = false;
	bool navVerrouJoystickCustom = false;

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

	int pendingX = 512;
	int pendingY = 512;
	bool hasNewJoystick = false;
	int encodeur = 0;
	int lastEncodeur = 0;

	NativeSerialPort serial;

	const float DEAD_ZONE_STANDARD = 0.5f;
	const float DEAD_ZONE_CUSTOM = 0.35f;

	const int SEUIL_HAUT_CUSTOM = 300;
	const int SEUIL_BAS_CUSTOM = 700;

	const Sint16 SEUIL_GACHETTE_CUSTOM = 10000;
};

#endif
