#ifndef RETICULE_H
#define RETICULE_H

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QElapsedTimer>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <iostream>
#include <algorithm>

#include <QCursor>
#include <QPoint>
#include <string>

#include "Variete.h"
#include "Touches.h"
#include "configuration_partie.h"

#include <SDL3/SDL.h>

using namespace std;

class Reticule : public QWidget
{
	Q_OBJECT
public:

	Reticule(QWidget* parent, const QPoint& pos, int choix, TypeManette manetteActive, Touches* touches);
	~Reticule();
	void setPosition(const QPoint& pos);
	string getPath(int choix) const;
	void moveJoystick(int x, int y, QWidget* parent);
	void moveJoystickPerso(int x, int y, QWidget* parent);

	// Appelé par EcranJeu::tick() après lirePerso() — synchronisé avec le rendu
	void applyJoystickPerso(QWidget* parent, float deltaMs);

	int getX() const;
	int getY() const;
	bool tirer() const { return touches->RTpressed(); }
	SDL_Gamepad* getGamepad()const { return gamepad; }
	int getChoixTir() const;
	int getArme() const { return choixTir; }

	Touches* getTouches() { return touches; }

	void ChangeReticule(QWidget* parent,int choix);

protected:
	void paintEvent(QPaintEvent* event) override;

private:

	QPixmap image;
	Touches* touches;
	TypeManette manetteActive = TypeManette::CLAVIER_SOURIS;

	int xini;
	int yini;

	int posX;
	int posY;

	int protJoystick = 2500;

	int hauteurEcran;
	int largeurEcran;

	SDL_Gamepad* gamepad = nullptr;

	int choixTir;

	// Accumulation des fractions de pixels pour eviter les skips
	float accumX = 0.0f;
	float accumY = 0.0f;

};

#endif