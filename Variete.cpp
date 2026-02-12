#include "Variete.h"

Variete::Variete()
{
	BasePath = QDir::currentPath().toStdString() + "/images/reticules/reticule";
}

Variete::~Variete() {
}

string Variete::findpath(int iteration)
{
	string path = BasePath + to_string(iteration) + ".png";
	return path;
}