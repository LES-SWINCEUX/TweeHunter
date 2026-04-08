#ifndef RETICULE_H
#define RETICULE_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <string>
#include <cmath>

#include "Variete.h"
#include "Touches.h"
#include "configuration_partie.h"

using namespace std;

class Reticule : public QWidget
{
	Q_OBJECT
public:
	Reticule(QWidget* parent, const QPoint& pos, int choix, TypeManette manetteActive, Touches* touches);
	~Reticule() = default;

	void setPosition(const QPoint& pos);
	string getPath(int choix) const;

	void applyJoystickPerso(QWidget* parent, float deltaMs);

	int getX() const;
	int getY() const;
	bool tirer() const { return touches->RTpressed(); }
	int getChoixTir() const;
	int getArme() const { return choixTir; }

	void ChangeReticule(QWidget* parent, int choix);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	void moveJoystick(float dx, float dy, QWidget* parent);
	void bornerPosition(QWidget* parent);

	QPixmap image;
	Touches* touches;
	TypeManette manetteActive = TypeManette::CLAVIER_SOURIS;

	int posX = 0;
	int posY = 0;

	int choixTir = 0;

	float accumX = 0.0f;
	float accumY = 0.0f;
};

#endif
