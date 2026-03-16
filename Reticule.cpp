#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix) : QWidget(parent)
{
	touches = new Touches();

	posX = pos.x();
	posY = pos.y();

	choixTir = choix;

	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AlwaysStackOnTop);
	setAttribute(Qt::WA_StaticContents); // evite les repaints du parent quand le reticule bouge

	image = QPixmap(QString::fromStdString(getPath(choix)));
	if (image.isNull()) {
		cout << "RETICULE::impossible de charger: " << endl;
		cout << getPath(1) << endl;
	}
	else {
		cout << "RETICULE::chargement réussi: " << endl;
	}

	setFixedSize(image.size());
	resize(parent->size());
	setPosition(pos);

	cout << touches->isJoystickConnected() << ": Reticule" << endl;

	// Timer pour la manette SDL officielle seulement
	if (touches->isJoystickConnected()) {
		QTimer* timer = new QTimer(this);
		timer->start(16); // ~60 Hz

		gamepad = touches->getGamepad();

		xini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
		yini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

		connect(timer, &QTimer::timeout, this, [=]() {
			if (!gamepad) return;
			SDL_UpdateGamepads();

			float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			// Deadzone
			if (std::fabs(x) < 0.1f) x = 0;
			if (std::fabs(y) < 0.1f) y = 0;

			if (sqrt((x - xini) * (x - xini) + (y - yini) * (y - yini)) > protJoystick) {
				moveJoystick(x, y, parent);
			}

			});
	}

	// Pas de timer ici pour la manette perso :
	// EcranJeu::tick() appelle lirePerso() + applyJoystickPerso() a chaque frame.
}

Reticule::~Reticule() {
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

	if (posX > (parent->width() - image.width() / 2)) posX = parent->width() - image.width() / 2;
	if (posX < -image.width() / 2) posX = -image.width() / 2;
	if (posY > parent->height() - image.height() / 2) posY = parent->height() - image.height() / 2;
	if (posY < -image.height() / 2) posY = -image.height() / 2;

	move(posX, posY);
	update();
}

void Reticule::moveJoystickPerso(int x, int y, QWidget* parent)
{
	int facteurRedu = 20;

	posX += ((x - 512) / facteurRedu);  // centre corrige: 512 au lieu de 500
	posY += ((y - 512) / facteurRedu);

	if (posX > (parent->width() - image.width() / 2)) posX = parent->width() - image.width() / 2;
	if (posX < -image.width() / 2) posX = -image.width() / 2;
	if (posY > parent->height() - image.height() / 2) posY = parent->height() - image.height() / 2;
	if (posY < -image.height() / 2) posY = -image.height() / 2;

	move(posX, posY);
	// Pas de update() ici — EcranJeu::tick() s'en charge
}

// Appelé par EcranJeu::tick() à chaque frame.
// Utilise le delta-time pour une vitesse constante peu importe le framerate.
void Reticule::applyJoystickPerso(QWidget* parent, float deltaMs)
{
	int x = touches->getxPerso();
	int y = touches->getyPerso();

	float dx = (x - 512) / 512.0f; // -1.0 a 1.0
	float dy = (y - 512) / 512.0f;

	// Deadzone
	if (sqrt(dx * dx + dy * dy) < 0.06f) return;

	float vitesse = 1.0f; // pixels par ms — ajuste selon ton gout

	// Accumule les fractions de pixels pour eviter les skips
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
	// lirePerso() retiré — appelé dans EcranJeu::tick() uniquement
}

int Reticule::getX() const
{
	return posX + image.width() / 2;
}

int Reticule::getY() const
{
	return posY + image.height() / 2;
}