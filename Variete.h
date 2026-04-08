#ifndef VARIETE_H	
#define VARIETE_H

#include <string>
#include <QDir>

class Variete
{
	public:
		Variete();
		~Variete();

		std::string findpath(int iteration);

	private:
		std::string BasePath;

};

#endif