#ifndef VARIETE_H	
#define VARIETE_H

#include <string>
#include <QDir>

using namespace std;

class Variete
{
	public:
		Variete();
		~Variete();

		string findpath(int iteration);


	private:
		string BasePath;

};




#endif