#ifndef TOUCHES_H
#define TOUCHES_H

#include <iostream>

#include <SDL3/SDL.h>
#include <QTimer>
#include <QDebug>
#include <cmath>

using namespace std;

class Touches
{
public:
	Touches();
	~Touches();

	bool isJoystickConnected() const { return joystick; }
	SDL_Gamepad* getGamepad()const { return gamepad; }

private:
	bool joystick;
	SDL_Gamepad* gamepad=nullptr;
	int middleX;
	int middleY;

};

#endif