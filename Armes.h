#ifndef ARMES_H
#define ARMES_H

#include <iostream>
#include <QPainterPath>

class Armes
{
public:
	Armes(int Arme = 1);
	QPainterPath choixArme(int choix, int x, int y) ;
	QPainterPath CreerContourTarte(int x, int y);
	int nbMunitions() const;

private:
	int ArmeActuelle;

};



#endif