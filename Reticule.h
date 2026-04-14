#ifndef RETICULE_H
#define RETICULE_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <string>
#include <cmath>
#include <iostream>

#include "Variete.h"
#include "Touches.h"
#include "configuration_partie.h"

class Reticule : public QWidget
{
	Q_OBJECT
public:
	Reticule(QWidget* parent, const QPoint& pos, int choix, TypeManette manetteActive, Touches* touches);
	~Reticule() = default;

	void setPosition(const QPoint& pos);
	std::string getPath(int choix) const;

	void applyJoystick(QWidget* parent, float dx, float dy, float deltaMs);


	int getX() const;
	int getY() const;
	bool tirer() const { return touches->RTpressed(); }
	int getChoixTir() const;
	int getArme() const { return choixTir; }

	void ChangeReticule(QWidget* parent, int choix);
	void ajoutReticule(QWidget* parent, QPaintEvent* ,int x, int y);
	void resetReticuleSuppl();


protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	void moveJoystick(float dx, float dy, QWidget* parent);
	void bornerPosition(QWidget* parent);

	QPixmap image;

	QPixmap image1;
	QPixmap image2;
	QPixmap image3;
	QPoint pos1;
	QPoint pos2;
	QPoint pos3;

	Touches* touches;
	TypeManette manetteActive = TypeManette::CLAVIER_SOURIS;

	int posX = 0;
	int posY = 0;

	int choixTir = 0;

	float accumX = 0.0f;
	float accumY = 0.0f;

};

#endif
