#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos, int choix) : QWidget(parent)
{
	xj = pos.x();
	yj = pos.y();

	posX = xj;//à changer pour une démarche plus propre
	posY = yj;


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

		connect(timer, &QTimer::timeout, this, [=]() {
			if (!gamepad) return;
			SDL_UpdateGamepads();

			float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			// Deadzone
			if (std::fabs(x) < 0.1f) x = 0;
			if (std::fabs(y) < 0.1f) y = 0;

			xj = x;
			yj = y;

			qDebug() << "X:" << x << "Y:" << y;

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

	//int newX = pos.x() - image.width() / 2;
	//int newY = pos.y() - image.height() / 2;
	//Très très temporaire : 
	
	if(xj<xini+100 && xj > xini - 100) {
		xj = 0;
	}
	
	if (yj < yini + 100 && yj>yini - 100) {
	
		yj = 0;
	}

	posX += (xj/25000);
	posY += (yj/25000);

	if (posX > 1000) posX = 1000;
	if (posX < 0) posX = 0;
	if (posY > 600) posY = 600;
	if (posY < 0) posY = 0;

	//cout << "posX: " << posX << " posY: " << posY << endl;


	//move(newX, newY);
	move(posX, posY);
	update();
}

void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this); // redessinage du widget lorsqu'il est mise à jour
	painter.drawPixmap(0, 0, image);
	setPosition(QPoint(0, 0));
}


