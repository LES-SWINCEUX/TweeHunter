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

using namespace std;

class Reticule : public QWidget
{
	Q_OBJECT
public:

	Reticule();
	Reticule(QWidget* parent, const QPoint& pos, int choix);
	~Reticule();
	void setPosition(const QPoint& pos);
	string getPath(int choix) const;

protected:
	void paintEvent(QPaintEvent* event) override;

private:

	QPixmap image;

};




#endif
