#ifndef ARMES_H
#define ARMES_H

#include <iostream>
#include <QPainterPath>
#include <QWidget>

#include "configuration_partie.h"

class Armes
{
public:
	Armes(ConfigurationPartie configuration);


	void setArmes(int arme) { ArmeActuelle = arme; }

	QPainterPath choixArme(int x, int y);

	QPainterPath Hitbox(int choix, int x, int y);
	QPainterPath Hitbox(PowerUpType choix, int x, int y);
	QPainterPath CreerContourTarte(int x, int y);
	int nbMunitions() const;
	int nbPowerUp() const;
	int addPowerUp();
	int getMult() { return mult; }
	void setMult(int value) { mult = value; }
	void reset7();


	int getArmeActuelle() const {
		return ArmeActuelle;
	}
	PowerUpType getPowerActuelle() const {
		return PowerActuelle;
	}

	void setFenetre(QWidget* fenetre) {
		p = fenetre;
	}

private:
	int ArmeActuelle;
	PowerUpType PowerActuelle = PowerUpType::GRENADE;
	QWidget* p = nullptr;
	int mult = 1;

	int x7[3] = { -1, -1, -1 };
	int y7[3] = { -1, -1, -1 };
};

#endif
