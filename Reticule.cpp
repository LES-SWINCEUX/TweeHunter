#include "Reticule.h"

Reticule::Reticule(QWidget* parent, const QPoint& pos) : QWidget(parent)
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AlwaysStackOnTop);

	image = QPixmap(QDir::currentPath() + "/images/reticules/reticule.png");
	if (image.isNull()) {
		cout << "RETICULE::impossible de charger: "<<endl;
	}
	else {
		//cout << "Perfect" << endl;
	}

	//Si je voulais redimentionner l'image du reticule:
	
	//QPixmap original("reticule.png");
	//int newWidth = 32;   // largeur souhaitée
	//int newHeight = 32;  // hauteur souhaitée
	//QPixmap scaled = original.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);



	setFixedSize(image.size());
	resize(parent->size());
	setPosition(pos);
}

Reticule::~Reticule() {
}

void Reticule::setPosition(const QPoint& pos)
{
	int newX = pos.x() - image.width() / 2;
	int newY = pos.y() - image.height() / 2;

	move(newX, newY);
	update();
}

void Reticule::paintEvent(QPaintEvent*)
{
	QPainter painter(this); // redessinage du widget lorsqu'il est mise à jour
	painter.drawPixmap(0, 0, image);
}


