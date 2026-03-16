#include "Touches.h"

Touches::Touches()
{

	//Initialisation des différentes méthodes de contrôle

    gamepad = nullptr;

    joystickOficiel = false;
    //Partie joystick de test
    //Initialisation du SDL ---
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0)
    {
        qDebug() << "Erreur SDL:" << SDL_GetError();
        return;
    }

    //Détection de la manette ---
    int count = 1;
    SDL_JoystickID* ids = SDL_GetGamepads(&count); // Liste des gamepads
    qDebug() << "Nombre de manettes detectees:" << count;


    if (ids && count > 0)
    {
        // Ouvrir la première manette
        gamepad = SDL_OpenGamepad(ids[0]);
        if (gamepad) {
            cout << "Manette ouverte !" << endl;
            joystickOficiel = true;
        }else
            cout << "Erreur ouverture:" << SDL_GetError();

        SDL_free(ids); // libérer la mémoire retournée par SDL_GetGamepads
    
    }
    else {
        cout << "Aucune manette" << endl;

    }

    serial.setPortName("COM3");        // port Arduino
    serial.setBaudRate(115200);

    joystickPerso = false;

    if (serial.open(QIODevice::ReadOnly)) {
		cout << "Port série ouvert !" << endl;
		joystickPerso = true;
        /*connect(&serial, &QSerialPort::readyRead,
            this);*/
    }
    else {

		cout << "Aucun port série" << endl;
    }

}

Touches::~Touches() {
}

bool Touches::RTpressed() const {//Retourne si le bouton RT est pressé ou non
    Sint16 value = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (value > 10000) {
		return true;
    }

	return false;
}

void Touches::lirePerso() {// fonction appelée pour lire les données de la manette personalisé ----> (À appeler avant chaque fois que l'on veux chercher une donnée de la manette perso) 

	while (serial.canReadLine()) //lit chaque ligne disponible du port série tant qu'il y en a, et traite les données reçues (met à jour les données de la classe Touches)
    {
		QJsonDocument doc = QJsonDocument::fromJson(serial.readLine()); //récupère la prochaine ligne du port série et la convertit en document JSON

        if (!doc.isNull())
        {
            QJsonObject obj = doc.object();

            if (obj["type"] == "joystick") {// Lit les données du joystick
                x = obj["x"].toInt();
                y = 1023 - obj["y"].toInt();

                qDebug() << "Joystick:" << x << y;

            }
            else if (obj["type"] == "event") {// Lit les données des gachette
                cout << obj["btn"].toInt() << endl;
                if (obj["btn"].toInt() == 1) {
                    gachette = true;
                }
                else {
                    gachette = false;
				}

                //qDebug() << "gachette:" << gachette;
			}else if (obj["type"] == "reload") {// Lit les données des reload

                if (obj["reload"].toInt() == 1) {
                    reload = true;
                }
                else {
                    reload = false;
                }
                qDebug() << "reload:" << reload;

            }else if (obj["type"] == "accelerometre") {// Lit les données de la accelerometre
                if (obj["accelerometre"].toInt() == 1) {
                    accelerometre = true;
                }
                else {
                    accelerometre = false;
                }
                qDebug() << "accelerometre:" << accelerometre;
			}
            

        }
    }

}

//Éléments encore à implémenter pour la manette personnalisée:

//joystick, gachette (1,0), boutons_reload (1,0), accéléromêtre_shaké (1,0), possibilité de boutons supplémetaire

//renvoie nb_balles

int Touches::getxPerso() { //récupére la valeur x la plus à jour du joystick personnalisé

    return x;

}


int Touches::getyPerso() { //récupére la valeur y la plus à jour du joystick personnalisé
    QJsonObject Obj = QJsonDocument::fromJson(serial.readLine()).object();

    return y;
}
