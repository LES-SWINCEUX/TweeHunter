#ifndef TOUCHES_H
#define TOUCHES_H

#include <iostream>

#include <SDL3/SDL.h>
#include <QTimer>
#include <QDebug>
#include <cmath>

#include "NativeSerialPort.h"
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

class Touches : public QObject
{
	Q_OBJECT

public:
	Touches();
	~Touches();

	bool isJoystickConnected() const { return joystickOficiel; }
	bool isJoystickPersoConnected() const { return joystickPerso; }
	SDL_Gamepad* getGamepad()const { return gamepad; }
	bool RTpressed() const;

	void lirePerso();
	int getxPerso() const;
	int getyPerso() const;

	bool getGachette() const { return gachette; }
	bool getReload() const { return reload; }
	bool getAccelerometre() const { return accelerometre; }
	int getEncodeur() const { return encodeur; }


	int UseLastEncodeur();

    void envoyerNbBalles(int nbBalles);
	void envoyerMoteur();
    void envoyerRaw(const QByteArray& data);
	

private:
	bool joystickOficiel;
	bool joystickPerso;

	SDL_Gamepad* gamepad = nullptr;
	int middleX;
	int middleY;

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