#include "Touches.h"

Touches::Touches()
{
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

bool Touches::RTpressed() const {
    Sint16 value = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (value > 10000) {
		return true;
    }

	return false;
}

void Touches::lirePerso() {
    while (serial.canReadLine())
    {
        QJsonDocument doc = QJsonDocument::fromJson(serial.readLine());

        if (!doc.isNull())
        {
            QJsonObject obj = doc.object();

            if (obj["type"] == "joystick") {
                x = obj["x"].toInt();
                y = obj["y"].toInt();

                qDebug() << "Joystick:" << x << y;

            }
        }
    }

}


int Touches::getxPerso() {

    return x;

}


int Touches::getyPerso() {
    QJsonObject Obj = QJsonDocument::fromJson(serial.readLine()).object();

    return y;
}
