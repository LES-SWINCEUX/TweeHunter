#include "bush.h"

Bush::Bush(const QPointF& position, const QSizeF& taille, const QString& cheminSprite)
	: position(position), taille(taille)
{
	QString cheminResolu = QDir::currentPath() + "/images/Bush/JP.png";
	QSharedPointer<QPixmap> pix = SpriteManager::instance().getPixmap(cheminResolu);

	if (pix && !pix->isNull()) {
		SpriteSheet sheet(pix, 4, 1);
		sprite.setSprite(sheet);
		sprite.setCycle(1000);

	}
}

void Bush::dessiner(QPainter& painter)
{
	if (!sprite.estValide()) {
		return;
	}

	QRect dest(static_cast<int>(position.x() - taille.width() / 2),
		static_cast<int>(position.y() - taille.height() / 2), static_cast<int>(taille.width()), static_cast<int>(taille.height()));

	sprite.dessiner(painter, dest, 0, false, false);

}