#include "Touches.h"

Touches::Touches(): joystickPerso(false), middleX(0), middleY(0)
{
    gamepad = nullptr;

    joystickOficiel = false;

    if (SDL_Init(SDL_INIT_GAMEPAD) < 0)
    {
        qDebug() << "Erreur SDL:" << SDL_GetError();
        return;
    }

    int count = 1;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    qDebug() << "Nombre de manettes detectees:" << count;


    if (ids && count > 0)
    {
        gamepad = SDL_OpenGamepad(ids[0]);
        SDL_free(ids);

        if (gamepad) {
            joystickOficiel = true;
        }
    }

    QString arduinoPort = NativeSerialPort::findArduinoPort();

    joystickPerso = false;

    if (!arduinoPort.isEmpty()) {        
        serial.setPortName(arduinoPort);
        serial.setBaudRate(115200);

        if (serial.open(NativeSerialPort::ReadWrite)) {
            joystickPerso = true;
        }
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

        if (doc.isNull()) {
            continue;
        }

        QJsonObject obj = doc.object();

        if (obj["type"] == "joystick") {
            pendingX = obj["x"].toInt();
            pendingY = 1023 - obj["y"].toInt();
            hasNewJoystick = true;
        }
        else if (obj["type"] == "event") {
            if (obj["btn"].toInt() == 1) {
                gachette = true;
            }
            else {
                gachette = false;
            }

            if (obj["btn1"].toInt() == 1) {
                reload = true;
            }
            else {
                reload = false;
            }

            if (obj["btn2"].toInt() == 1) {
                accelerometre = true;
            }
            else {
                accelerometre = false;
            }

            encodeur = obj["encodeur"].toInt();

            if (encodeur != 0) {
                lastEncodeur = encodeur;
            }
        }
    }

    if (hasNewJoystick) {
        x = pendingX;
        y = pendingY;
        hasNewJoystick = false;
    }
}

int Touches::UseLastEncodeur() {
    int temp = lastEncodeur;
    if (temp != 0) {
        lastEncodeur = 0;
    }
    return temp;
}

int Touches::getxPerso() const {
    return x;
}


int Touches::getyPerso() const {
    return y;
}

void Touches::envoyerNbBalles(int nbBalles)
{
    if (!serial.isOpen()) {
        return;
    }

    QJsonObject obj;
    obj["type"] = "config";
    obj["nb_balles"] = nbBalles;

    QByteArray msg = "\n" + QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    serial.write(msg);
}

void Touches::envoyerRaw(const QByteArray& data)
{
    if (!serial.isOpen()) {
        return;
    }
    serial.write(data);
}

void Touches::envoyerMoteur()
{
    if (!serial.isOpen()) {
        return;
    }

    QJsonObject obj;
    obj["type"] = "config";
    obj["moteur"] = 1;
	QByteArray msg = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    serial.write(msg);
}

void Touches::envoyerFinPartie() {
    // TODO: envoyer signal de fin de partie pour activer le swinceur
    qDebug("Fin de la partie");
}