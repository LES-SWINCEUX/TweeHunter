#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix, TypeManette manetteActive_, Touches* t)
	: QWidget(parent), manetteActive(manetteActive_)
{
	touches = t;

	posX = pos.x();
	posY = pos.y();

	choixTir = choix;

	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AlwaysStackOnTop);
	setAttribute(Qt::WA_StaticContents);

	ChangeReticule(parent, choixTir);

	if (manetteActive == TypeManette::STANDARD && touches && touches->isJoystickConnected()) {
		QTimer* timer = new QTimer(this);
		timer->start(16);

		connect(timer, &QTimer::timeout, this, [=]() {
			touches->mettreAJour();

			float x = touches->axeX();
			float y = touches->axeY();

			if (std::fabs(x) < 0.1f) {
				x = 0.0f;
			}

			if (std::fabs(y) < 0.1f) {
				y = 0.0f;
			}

			moveJoystick(x, y, parent);
		});
	}
}

void Reticule::ChangeReticule(QWidget* parent, int choix)
{
	QPoint pos(getX(), getY());

	choixTir = choix;
	image = QPixmap(QString::fromStdString(getPath(choix)));

	setFixedSize(image.size());
	resize(parent->size());

	setPosition(pos);
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

	move(posX, posY);
	update();
}

void Reticule::moveJoystick(float dx, float dy, QWidget* parent)
{
	// dx/dy sont normalisés [-1, 1] — vitesse en pixels par frame (à 60fps ~16ms)
	const float vitesse = 28.0f;
	posX += int(dx * vitesse);
	posY += int(dy * vitesse);

	bornerPosition(parent);
	move(posX, posY);
	update();
}

void Reticule::applyJoystickPerso(QWidget* parent, float deltaMs)
{
	float dx = touches->customAxeX();
	float dy = touches->customAxeY();

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
	move(posX, posY);
}

void Reticule::bornerPosition(QWidget* parent)
{
	posX = qBound(-image.width() / 2,  posX, parent->width()  - image.width()  / 2);
	posY = qBound(-image.height() / 2, posY, parent->height() - image.height() / 2);
}

void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, image);
}

int Reticule::getX() const
{
	return posX + image.width() / 2;
}

int Reticule::getY() const
{
	return posY + image.height() / 2;
}
