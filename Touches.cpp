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
        }
        else
            cout << "Erreur ouverture:" << SDL_GetError();

        SDL_free(ids); // libérer la mémoire retournée par SDL_GetGamepads

    }
    else {
        cout << "Aucune manette" << endl;

    }

    QString arduinoPort = NativeSerialPort::findArduinoPort();

    joystickPerso = false;

    if (arduinoPort.isEmpty()) {
        cout << "Aucun Arduino detecte sur un port serie" << endl;
    }
    else {
        cout << "Arduino detecte sur : " << arduinoPort.toStdString() << endl;
        serial.setPortName(arduinoPort);
        serial.setBaudRate(115200);

        if (serial.open(NativeSerialPort::ReadWrite)) {
            cout << "Port serie ouvert !" << endl;
            joystickPerso = true;
        }
        else {
            cout << "Impossible d'ouvrir le port serie" << endl;
        }
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

    while (serial.canReadLine())
    {
        QJsonDocument doc = QJsonDocument::fromJson(serial.readLine());

        if (!doc.isNull())
        {
            QJsonObject obj = doc.object();

            if (obj["type"] == "joystick") {// Lit les données du joystick
                // On ecrase l'ancienne valeur — seule la derniere compte
                pendingX = obj["x"].toInt();
                pendingY = 1023 - obj["y"].toInt();
                hasNewJoystick = true;
            }
            else if (obj["type"] == "event") {// Lit les données des gachette
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
                    cout << encodeur << endl;
                    lastEncodeur = encodeur;
                }

                //qDebug() << "gachette:" << gachette;
			//}else if (obj["type"] == "event1") {// Lit les données des reload

   //             if (obj["btn1"].toInt() == 1) {
   //                 reload = true;
   //             }
   //             else {
   //                 reload = false;
   //             }
   //             qDebug() << "reload:" << reload;

   //         }else if (obj["type"] == "event2") {// Lit les données de la accelerometre
   //             if (obj["btn2"].toInt() == 1) {
   //                 accelerometre = true;
   //             }
   //             else {
   //                 accelerometre = false;
   //             }
   //             qDebug() << "accelerometre:" << accelerometre;
			}
            

        }
    }

    if (hasNewJoystick) {
        x = pendingX;
        y = pendingY;
        hasNewJoystick = false;
    }
}

//Éléments encore à implémenter pour la manette personnalisée:

//joystick, gachette (1,0), boutons_reload (1,0), accéléromêtre_shaké (1,0), possibilité de boutons supplémetaire

//renvoie nb_balles

int Touches::UseLastEncodeur() {
    int temp = lastEncodeur;
    if (temp != 0) {
        lastEncodeur = 0;
    }
    return temp;
}

int Touches::getxPerso() { //récupére la valeur x la plus à jour du joystick personnalisé

    return x;

}


int Touches::getyPerso() { //récupére la valeur y la plus à jour du joystick personnalisé
    return y;
}

void Touches::envoyerNbBalles(int nbBalles)
{
    if (!serial.isOpen()) return;

    QJsonObject obj;
    obj["type"]     = "config";
    obj["nb_balles"] = nbBalles;

    QByteArray msg = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    serial.write(msg);
}

void Touches::envoyerRaw(const QByteArray& data)
{
    if (!serial.isOpen()) return;
    serial.write(data);
}

void Touches::envoyerMoteur()
{
	if (!serial.isOpen()) return;
    QJsonObject obj;
    obj["type"] = "config";
    obj["moteur"] = 1;
	QByteArray msg = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    serial.write(msg);
}