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

using namespace std;

class Reticule : public QWidget
{
	Q_OBJECT
public:

	Reticule(QWidget* parent, const QPoint& pos, int choix);
	~Reticule();
	void setPosition(const QPoint& pos);
	string getPath(int choix) const;
	void moveJoystick(int x, int y, QWidget* parent);
	int getX() const;
	int getY() const;
	bool tirer() const { return touches.RTpressed(); }

protected:
	void paintEvent(QPaintEvent* event) override;

private:

	QPixmap image;
	Touches touches;

	int xini;
	int yini;

	int posX;
	int posY;

	int protJoystick=2500;

	int hauteurEcran;
	int largeurEcran;


};




#endif
