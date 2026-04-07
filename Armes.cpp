#include "Armes.h"


Armes::Armes(ConfigurationPartie configuration)
{
	ArmeActuelle = configuration.arme;
	PowerActuelle = configuration.powerUp;
}

QPainterPath Armes::choixArme(int x, int y) {
	return Hitbox(ArmeActuelle, x, y);
}

QPainterPath Armes::Hitbox(PowerUpType choix, int x, int y) {
	QPainterPath Hitbox;
	switch (choix) {
		case PowerUpType::GRENADE:
			Hitbox.addEllipse(QPointF(x, y), 300, 300);
			return Hitbox;
		case PowerUpType::ZAP:
			Hitbox.addEllipse(QPointF(x, y), 10 * mult, 10 * mult);
			return Hitbox;
		case PowerUpType::MITRAILLETTE:
			return this->Hitbox(ArmeActuelle, x, y);
		case PowerUpType::TACTICAL_NUKE:
			if (p) {
				Hitbox.addRect(0, 0, p->width(), p->height());
			}
			return Hitbox;
		default:
			return Hitbox;
	}
}

QPainterPath Armes::Hitbox(int choix, int x, int y)
{
	QPainterPath Hitbox;
	int largeur = 120;
	int hauteur = 195;

	switch (choix) {
	case 1:
		//cout << "Pistolet sélectionné" << endl;
		Hitbox.addEllipse(QPointF(x, y), 30, 30);
		return Hitbox;
	case 2:
		//cout << "Fusil à pompe sélectionné" << endl;
		Hitbox.addEllipse(QPointF(x, y), 60, 60);
		return Hitbox;
	case 3:
		//cout << "Gros fusil petit tir sélectionné" << endl;
		Hitbox.addEllipse(QPointF(x, y), 10, 10);
		return Hitbox;
	case 4:
		//cout << "Bombardement sélectionné" << endl;
		Hitbox.addEllipse(QPointF(x, y), 30, 30);
		Hitbox.addEllipse(QPointF(x, y+ 90), 30, 30);
		Hitbox.addEllipse(QPointF(x, y - 90), 30, 30);
		Hitbox.addEllipse(QPointF(x - 90, y), 30, 30);
		Hitbox.addEllipse(QPointF(x + 90, y), 30, 30);
		return Hitbox;
	case 5:
		//cout << "Bombardement sélectionné" << endl;
		Hitbox = CreerContourTarte(x-150,y-176/2);
		return Hitbox;
	case 6:
		// Swince : comportement temporaire en attendant un réticule dédié
		Hitbox.addRect(x-(largeur/2), y-(hauteur/2), largeur, hauteur);
		return Hitbox;
	default:
		return Hitbox;
	}
}

int Armes::nbMunitions() const
{
	switch (ArmeActuelle) {
	case 1:
		return 9;
	case 2:
		return 5;
	case 3:
		return 1;
	case 4:
		return 6;
	case 5:
		return 9;
	case 6:
		return 8;
	default:
		return 0;
	}
}

int Armes::nbPowerUp() const {
	switch (PowerActuelle) {
	case PowerUpType::GRENADE:
		return 5;
	case PowerUpType::ZAP:
		return 9;
	case PowerUpType::MITRAILLETTE:
		return 2;
	case PowerUpType::TACTICAL_NUKE:
		return 1;
	default:
		return 0;
	}
}

QPainterPath Armes::CreerContourTarte(int x, int y)
{
	QPainterPath path;

	double largeur = 300;
	double hauteur = 120;
	double profondeur = 60;


	QRectF dessus(x, y, largeur, hauteur);
	path.addEllipse(dessus);


	QPainterPath avant;
	avant.moveTo(x, y + hauteur / 2);

	avant.cubicTo(
		x + largeur * 0.25, y + hauteur + profondeur,
		x + largeur * 0.75, y + hauteur + profondeur,
		x + largeur, y + hauteur / 2
	);

	path.addPath(avant);


	path.moveTo(x, y + hauteur / 2);
	path.lineTo(x, y + hauteur / 2 + profondeur);

	path.moveTo(x + largeur, y + hauteur / 2);
	path.lineTo(x + largeur, y + hauteur / 2 + profondeur);


	QPainterPath croute;

	int vagues = 20;
	double step = largeur / vagues;

	croute.moveTo(x, y + hauteur / 2);

	for (int i = 0; i < vagues; i++)
	{
		double x1 = x + i * step;
		double x2 = x + (i + 1) * step;
		double ctrlX = (x1 + x2) / 2.0;
		double ctrlY = y + hauteur / 2 + (i % 2 == 0 ? -12 : 12);

		croute.quadTo(ctrlX, ctrlY, x2, y + hauteur / 2);
	}

	path.addPath(croute);

	return path;
}
