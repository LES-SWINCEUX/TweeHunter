#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix) : QWidget(parent)
{

	posX = pos.x();
	posY = pos.y();


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

		SDL_Gamepad* gamepad = touches.getGamepad();

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
			
			

			//qDebug() << "X:" << x << "Y:" << y;

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

	if (posX > (parent->width()-image.width())) posX = parent->width()-image.width();
	if (posX < 0) posX = 0;
	if (posY > parent->height()-image.height()) posY = parent->height()-image.height();
	if (posY < 0) posY = 0;

	move(posX, posY);
	update();
}


void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this); // redessinage du widget lorsqu'il est mise à jour
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

