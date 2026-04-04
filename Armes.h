#ifndef ARMES_H
#define ARMES_H

#include <iostream>
#include <QPainterPath>
#include <QWidget>

using namespace std;

class Armes
{
public:
	Armes(int Arme = 1, int PowerUp = 1, QWidget* parent=nullptr );

	QPainterPath choixArme(int x, int y) ;
	QPainterPath choixPowerUp(int x, int y, int specification = 0);

	QPainterPath Hitbox(int choix, int x, int y);
	QPainterPath CreerContourTarte(int x, int y);
	int nbMunitions() const;
	int nbPowerUp() const;

	int getArmeActuelle() const {
		return ArmeActuelle;
	}
	int getPowerActuelle() const {
		return PowerActuelle;
	}


private:
	int ArmeActuelle;
	int PowerActuelle;
	QWidget* p;

	int mult = 1;

};



#endif