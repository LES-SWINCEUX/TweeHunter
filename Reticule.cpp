#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix) : QWidget(parent)
{

	posX = pos.x();
	posY = pos.y();

	choixTir = choix;

	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AlwaysStackOnTop);

	image = QPixmap(QString::fromStdString(getPath(choix)));
	if (image.isNull()) {
		cout << "RETICULE::impossible de charger: "<<endl;
		cout << getPath(1) << endl;
	}
	else {
		cout << "RETICULE::chargement réussi: " << endl;
	}

	//Si je voulais redimentionner l'image du reticule:
	
	//QPixmap original("reticule.png");
	//int newWidth = 32;   // largeur souhaitée
	//int newHeight = 32;  // hauteur souhaitée
	//QPixmap scaled = original.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	
	setFixedSize(image.size());
	resize(parent->size());
	setPosition(pos);

	cout << touches.isJoystickConnected() << ": Reticule" << endl;
	if (touches.isJoystickConnected()) {
		QTimer* timer = new QTimer(this);
		timer->start(16); // ~60 Hz

		gamepad = touches.getGamepad();

		xini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) ; //initialisation du point central du joystick
		yini = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

		connect(timer, &QTimer::timeout, this, [=]() {// prise des données du joystick
			if (!gamepad) return;
			SDL_UpdateGamepads();

			float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			// Deadzone
			if (std::fabs(x) < 0.1f) x = 0;
			if (std::fabs(y) < 0.1f) y = 0;

			
			if (sqrt((x-xini)*(x - xini)+(y-yini)*(y-yini))>protJoystick) { //protection contre les joystick mal calibrés
				moveJoystick(x, y, parent);
			}
			
			

			qDebug() << "Axe du joystick ---> X:" << x << "Y:" << y;

			});

	}
	if(touches.isJoystickPersoConnected()) {
		QTimer* timer = new QTimer(this);
		timer->start(16); // ~60 Hz
		connect(timer, &QTimer::timeout, this, [=]() {// prise des données du joystick
			touches.lirePerso();	
			int x = touches.getxPerso();
			int y = touches.getyPerso();
			// Deadzone

			qDebug() << "Axe du joystick perso ---> X:" << x << "Y:" << y;

			if (sqrt((x - 500) * (x - 500) + (y - 500) * (y - 500)) > 5) { //protection contre les joystick mal calibrés
				moveJoystickPerso(x, y, parent);
			}
			});
	}
	
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
	cout << "Position du reticule: x=" << posX << " y=" << posY << endl;
	update();
}

void Reticule::moveJoystick(int x, int y, QWidget* parent)
{
	int facteurRedu = 1000;
	posX += (x / facteurRedu);
	posY += (y / facteurRedu);

	if (posX > (parent->width()-image.width()/2)) posX = parent->width()-image.width()/2;
	if (posX < -image.width()/2) posX = -image.width()/2;
	if (posY > parent->height()-image.height()/2) posY = parent->height()-image.height()/2;
	if (posY < -image.height()/2) posY = -image.height()/2;

	move(posX, posY);
	cout << "Position du reticule: x=" << posX << " y=" << posY << endl;
	update();
}

void Reticule::moveJoystickPerso(int x, int y, QWidget* parent) {
	int facteurRedu = 20;

	cout << "moveJoystickPerso: x=" << x << " y=" << y << endl;
	posX += ((x-500) / facteurRedu);
	posY += ((y-500) / facteurRedu);

	if (posX > (parent->width() - image.width() / 2)) posX = parent->width() - image.width() / 2;
	if (posX < -image.width() / 2) posX = -image.width() / 2;
	if (posY > parent->height() - image.height() / 2) posY = parent->height() - image.height() / 2;
	if (posY < -image.height() / 2) posY = -image.height() / 2;

	move(posX, posY);
	//cout << "Position du reticule: x=" << posX << " y=" << posY << endl;
	update();
}


void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this); // redessinage du widget lorsqu'il est mise à jour
	painter.drawPixmap(0, 0, image);
	touches.lirePerso();
}

int Reticule::getX() const
{ 
	return posX + image.width() / 2; 
}

int Reticule::getY() const 
{
return posY + image.height() / 2; 
}

