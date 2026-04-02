#ifndef ARMES_H
#define ARMES_H

#include <iostream>
#include <QPainterPath>
#include <QWidget>

class Armes
{
public:
	Armes(int Arme = 1, int PowerUp = 1, QWidget* parent=nullptr );

	QPainterPath choixArme(int x, int y) ;
	QPainterPath choixPowerUp(int x, int y);

	QPainterPath Hitbox(int choix, int x, int y);
	QPainterPath CreerContourTarte(int x, int y);
	int nbMunitions() const;
	int nbPowerUp() const;


private:
	int ArmeActuelle;
	int PowerActuelle;
	QWidget* p;

};



#endif