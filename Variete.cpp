#include "Variete.h"

Variete::Variete()
{
	BasePath = QDir::currentPath().toStdString() + "/images/reticules/reticule";
}

Variete::~Variete() {
}

std::string Variete::findpath(int iteration)
{
	std::string path = BasePath + std::to_string(iteration) + ".png";
	return path;
}