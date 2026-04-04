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

		gamepad = touches->getGamepad();

		if (gamepad) {
			xini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			yini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
		}

		connect(timer, &QTimer::timeout, this, [=]() {
			if (!gamepad) return;

			SDL_UpdateGamepads();

			float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			if (std::fabs(x) < 0.1f) x = 0;
			if (std::fabs(y) < 0.1f) y = 0;

			if (std::sqrt((x - xini) * (x - xini) + (y - yini) * (y - yini)) > protJoystick) {
				moveJoystick(x, y, parent);
			}
		});
	}
}

Reticule::~Reticule() {
}

void Reticule::ChangeReticule(QWidget* parent,int choix) {
	QPoint pos(getX(), getY());

	choixTir = choix;
	image = QPixmap(QString::fromStdString(getPath(choix)));

	setFixedSize(image.size());
	resize(parent->size());

	setPosition(pos);

}


string Reticule::getPath(int choix) const
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

void Reticule::moveJoystick(int x, int y, QWidget* parent)
{
	int facteurRedu = 1000;
	posX += (x / facteurRedu);
	posY += (y / facteurRedu);

	if (posX > (parent->width() - image.width() / 2)) {
		posX = parent->width() - image.width() / 2;
	}

	if (posX < -image.width() / 2) {
		posX = -image.width() / 2;
	}

	if (posY > parent->height() - image.height() / 2) {
		posY = parent->height() - image.height() / 2;
	}

	if (posY < -image.height() / 2) {
		posY = -image.height() / 2;
	}

	move(posX, posY);
	update();
}

void Reticule::moveJoystickPerso(int x, int y, QWidget* parent)
{
	int facteurRedu = 20;

	posX += ((x - 512) / facteurRedu);
	posY += ((y - 512) / facteurRedu);

	if (posX > (parent->width() - image.width() / 2)) {
		posX = parent->width() - image.width() / 2;
	}

	if (posX < -image.width() / 2) {
		posX = -image.width() / 2;
	}

	if (posY > parent->height() - image.height() / 2) {
		posY = parent->height() - image.height() / 2;
	}

	if (posY < -image.height() / 2) {
		posY = -image.height() / 2;
	}

	move(posX, posY);
}

void Reticule::applyJoystickPerso(QWidget* parent, float deltaMs)
{
	int x = touches->getxPerso();
	int y = touches->getyPerso();

	float dx = (x - 512) / 512.0f;
	float dy = (y - 512) / 512.0f;

	if (sqrt(dx * dx + dy * dy) < 0.06f) return;

	float vitesse = 1.0f;

	accumX += dx * vitesse * deltaMs;
	accumY += dy * vitesse * deltaMs;

	int moveX = (int)accumX;
	int moveY = (int)accumY;
	accumX -= moveX;
	accumY -= moveY;

	posX += moveX;
	posY += moveY;

	posX = qBound(-image.width() / 2, posX, parent->width() - image.width() / 2);
	posY = qBound(-image.height() / 2, posY, parent->height() - image.height() / 2);

	move(posX, posY);
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