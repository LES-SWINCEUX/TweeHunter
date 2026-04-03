#ifndef ARMES_H
#define ARMES_H

#include <iostream>
#include <QPainterPath>

#include "configuration_partie.h"

class Armes
{
public:
	Armes(int Arme = 1, PowerUpType PowerUp = PowerUpType::GRENADE);

	QPainterPath choixArme(int x, int y) ;
	QPainterPath choixPowerUp(int x, int y);

	QPainterPath Hitbox(int choix, int x, int y);
	QPainterPath Hitbox(PowerUpType choix, int x, int y);
	QPainterPath CreerContourTarte(int x, int y);
	int nbMunitions() const;
	int nbPowerUp() const;

private:
	int ArmeActuelle;
	PowerUpType PowerActuelle = PowerUpType::GRENADE;

};

#endif