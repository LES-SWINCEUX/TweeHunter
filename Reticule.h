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

using namespace std;

class Reticule : public QWidget
{
	Q_OBJECT
public:

	Reticule(QWidget* parent, const QPoint& pos);
	~Reticule();
	void setPosition(const QPoint& pos);

protected:
	void paintEvent(QPaintEvent* event) override;

private:

	QPixmap image;

};




#endif
