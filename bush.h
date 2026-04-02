#ifndef BUSH_H
#define BUSH_H

#include <QPointF>
#include <QPainter>
#include <QSizeF>
#include "sprite.h"
#include "spritesheet.h"
#include "sprite_manager.h"
#include <QDir.h>

class Bush 
{
public:
	Bush(const QPointF& position, const QSizeF& taille, const QString& cheminSprite);

	void dessiner(QPainter& painter);
	QPointF getPosition() const {
		return position;
	}
	QSizeF getTaille() const {
		return taille;
	}
	bool estOccupee() const {
		return occupee;
	}
	void setOccupee(bool value) {
		occupee = value;
	}

private:
	QPointF position;
	QSizeF taille;
	Sprite sprite;
	bool occupee = false;
};
#endif