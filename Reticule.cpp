#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix, TypeManette manetteActive_, Touches* t)
	: QWidget(parent), manetteActive(manetteActive_)
{
	touches = t;

	posX = pos.x();
	posY = pos.y();

	choixTir = choix;
	choixTir = 7;


	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AlwaysStackOnTop);
	setAttribute(Qt::WA_StaticContents);

	ChangeReticule(parent, choixTir);

	if (parent)
		setGeometry(0, 0, parent->width(), parent->height());
}

void Reticule::ChangeReticule(QWidget* parent, int choix)
{
	QPoint pos(getX(), getY());

	choixTir = choix;
	image = QPixmap(QString::fromStdString(getPath(choix)));

	setPosition(pos);
}

void Reticule::resetReticuleSuppl() {
	image1= QPixmap();
	image2= QPixmap();
	image3 = QPixmap();
}


void Reticule::ajoutReticule(QWidget* parent, QPaintEvent*,int x, int y) {
	if (image1.isNull()) {
		image1 = QPixmap(QString::fromStdString(getPath(choixTir)));
		pos1 = QPoint(x - image1.width() / 2, y - image1.height() / 2);

		std::cout << "ajout reticule 1" << std::endl;
		
	}
	else if (image2.isNull()) {
		image2 = QPixmap(QString::fromStdString(getPath(choixTir)));
		pos2 = QPoint(x - image2.width() / 2, y - image2.height() / 2);

	}
	else if (image3.isNull()) {
		image3 = QPixmap(QString::fromStdString(getPath(choixTir)));
		pos3 = QPoint(x - image3.width() / 2, y - image3.height() / 2);
	}
	update();
}

std::string Reticule::getPath(int choix) const
{
	Variete v;
	return v.findpath(choix);
}

int Reticule::getChoixTir() const
{
	return choixTir;
}

void Reticule::setPosition(const QPoint& pos)
{
	posX = pos.x() - image.width() / 2;
	posY = pos.y() - image.height() / 2;

	update();
}

void Reticule::moveJoystick(float dx, float dy, QWidget* parent)
{
	// dx/dy sont normalisés [-1, 1] — vitesse en pixels par frame (à 60fps ~16ms)
	const float vitesse = 28.0f;
	posX += int(dx * vitesse);
	posY += int(dy * vitesse);

	bornerPosition(parent);
	update();
}

void Reticule::applyJoystick(QWidget* parent, float dx, float dy, float deltaMs)
{
	if (manetteActive == TypeManette::STANDARD) {
		if (std::fabs(dx) < 0.1f) {
			dx = 0.0f;
		}

		if (std::fabs(dy) < 0.1f) {
			dy = 0.0f;
		}

		moveJoystick(dx, dy, parent);
	} 
	else {
		if (std::sqrt(dx * dx + dy * dy) < 0.06f) {
			return;
		}

		const float vitesse = 1.0f;

		accumX += dx * vitesse * deltaMs;
		accumY += dy * vitesse * deltaMs;

		int moveX = int(accumX);
		int moveY = int(accumY);
		accumX -= moveX;
		accumY -= moveY;

		posX += moveX;
		posY += moveY;

		bornerPosition(parent);
		update();
	}
}

void Reticule::bornerPosition(QWidget* parent)
{
	posX = qBound(-image.width() / 2,  posX, parent->width()  - image.width()  / 2);
	posY = qBound(-image.height() / 2, posY, parent->height() - image.height() / 2);
}

void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(posX, posY, image);
	if (!image1.isNull())
		painter.drawPixmap(pos1, image1);
	if (!image2.isNull())
		painter.drawPixmap(pos2, image2);
	if (!image3.isNull())
		painter.drawPixmap(pos3, image3);
}

void Reticule::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	if (parentWidget())
		setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
}

int Reticule::getX() const
{
	return posX + image.width() / 2;
}

int Reticule::getY() const
{
	return posY + image.height() / 2;
}
