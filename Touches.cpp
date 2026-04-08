#include "Touches.h"

Touches::Touches(): joystickPerso(false), middleX(0), middleY(0)
{
    gamepad = nullptr;

    joystickOfficiel = false;

    SDL_PumpEvents();

    int count = 1;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    qDebug() << "Nombre de manettes detectees:" << count;


    if (ids && count > 0)
    {
        gamepad = SDL_OpenGamepad(ids[0]);
        SDL_free(ids);

        if (gamepad) {
            joystickOfficiel = true;
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
    if (gamepad) {
        SDL_CloseGamepad(gamepad);
        gamepad = nullptr;
    }
    if (serial.isOpen()) {
        serial.close();
    }
}

bool Touches::RTpressed() const {

    Sint16 value = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (value > 10000) {
        return true;
    }

    return false;
}

bool Touches::LTpressed() const {

    Sint16 value = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);

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

void Touches::mettreAJour()
{
    if (joystickOfficiel && gamepad) {
        SDL_UpdateGamepads();
    }
    if (joystickPerso) {
        lirePerso();
    }
}

bool Touches::haut() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP) || axeY() < -kDeadZoneStandard;
}

bool Touches::bas() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN) || axeY() > kDeadZoneStandard;
}

bool Touches::gauche() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT) || axeX() < -kDeadZoneStandard;
}

bool Touches::droite() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) || axeX() > kDeadZoneStandard;
}

bool Touches::confirmer() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) || SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);
}

bool Touches::retour() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST) || SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_BACK);
}

float Touches::axeX() const
{
    if (!gamepad) {
        return 0.0f;
    }

    return std::clamp(float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX)) / 32767.0f, -1.0f, 1.0f);
}

float Touches::axeY() const
{
    if (!gamepad) {
        return 0.0f;
    }

    return std::clamp(float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / 32767.0f, -1.0f, 1.0f);
}

bool Touches::customHaut() const
{
    return y < kSeuilCustomHaut;
}

bool Touches::customBas() const
{
    return y > kSeuilCustomBas;
}

bool Touches::customGauche() const
{
    return customAxeX() < -kDeadZoneCustom;
}

bool Touches::customDroite() const
{
    return customAxeX() > kDeadZoneCustom;
}

float Touches::customAxeX() const
{
    return std::clamp((float(x) - 512.0f) / 512.0f, -1.0f, 1.0f);
}

float Touches::customAxeY() const
{
    return std::clamp((float(y) - 512.0f) / 512.0f, -1.0f, 1.0f);
}

void Touches::lireNavigation()
{
    mettreAJour();

    if (joystickOfficiel && gamepad) {
        const bool h = haut();
        const bool b = bas();
        const bool g = gauche();
        const bool d = droite();
        const bool ok = confirmer();
        const bool re = retour();

        // Dead zone joystick pour éviter les répétitions
        const bool enDeadZone = std::abs(axeX()) < kDeadZoneStandard && std::abs(axeY()) < kDeadZoneStandard;
        if (enDeadZone) navVerrouJoystick = false;

        if (h && !navHautPrecedent) {
            emit naviguerHaut();
        }

        if (b && !navBasPrecedent) {
            emit naviguerBas();
        }

        if (g && !navGauchePrecedent) {
            emit naviguerGauche();
        }

        if (d && !navDroitePrecedent) {
            emit naviguerDroite();
        }

        if (ok && !navOkPrecedent) {
            emit naviguerConfirmer();
        }

        if (re && !navRetourPrecedent) {
            emit naviguerRetour();
        }

        navHautPrecedent = h;
        navBasPrecedent = b;
        navGauchePrecedent = g;
        navDroitePrecedent = d;
        navOkPrecedent = ok;
        navRetourPrecedent = re;
    }

    // --- Manette custom ---
    if (joystickPerso) {
        const bool ch = customHaut();
        const bool cb = customBas();
        const bool cg = customGauche();
        const bool cd = customDroite();
        const bool co = customConfirmer();

        const bool enDeadZone = !cg && !cd && !ch && !cb;

        if (enDeadZone) {
            navVerrouJoystickCustom = false;
        }

        if (!navVerrouJoystickCustom) {
            if (ch && !navCustomHautPrecedent) { 
                emit naviguerHaut();   
                navVerrouJoystickCustom = true; 
            }

            if (cb && !navCustomBasPrecedent) { 
                emit naviguerBas();
                navVerrouJoystickCustom = true;
            }
            if (cg && !navCustomGauchePrecedent) {
                emit naviguerGauche(); 
                navVerrouJoystickCustom = true; 
            }

            if (cd && !navCustomHautPrecedent) { 
                emit naviguerDroite(); 
                navVerrouJoystickCustom = true; 
            }
        }

        if (co && !navCustomOkPrecedent) {
            emit naviguerConfirmer();
        }

        navCustomHautPrecedent = ch;
        navCustomBasPrecedent = cb;
        navCustomGauchePrecedent = cg;
        navCustomOkPrecedent = co;
    }
}

void Touches::lireJeu(TypeManette manette, qint64 deltaMs)
{
    mettreAJour();

    switch (manette) {
    case TypeManette::STANDARD: {
        if (!gamepad) return;

        const bool gachetteTir = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > kSeuilGachette;
        const bool gachettePowerUp = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)  > kSeuilGachette;
        const bool reloadBtn = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST);
        const bool startBtn = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);

        if (gachetteTir && !gachetteTirPrecedente) {
            emit tireDemande();
        }

        if (gachettePowerUp && !gachettePowerUpPrecedente) {
            emit powerUpDemande();
        }

        if (reloadBtn && !reloadPrecedent) {
            emit reloadDemande();
        }

        if (startBtn && !startPrecedent) {
            emit pauseDemande();
        }

        gachetteTirPrecedente = gachetteTir;
        gachettePowerUpPrecedente = gachettePowerUp;
        reloadPrecedent = reloadBtn;
        startPrecedent = startBtn;
        break;
    }
    case TypeManette::CUSTOM: {
        if (!isJoystickPersoConnected()) {
            return;
        }

        if (customConfirmer() && !reload) {
            if (!gachetteTirPrecedente) {
                emit tireDemande();
            }
            gachetteTirPrecedente = true;
        } else if (!customConfirmer()) {
            gachetteTirPrecedente = false;
        }

        if (reload && getAccelerometre()) {
            emit reloadDemande();
        }

        if (reload && getEncodeur() != 0) {
            emit pauseDemande();
        }

        if (customConfirmer() && reload) {
            if (!powerUpActif) {
                powerUpActif = true;
                emit powerUpDemande();
            }
        } else {
            powerUpActif = false;
        }

        emit joystickDeplace(float(deltaMs));
        break;
    }
    default:
        break;
    }
}

void Touches::verifierConnexion()
{
    // Re-détecter la manette standard si elle n'était pas connectée
    if (!joystickOfficiel || !gamepad) {
        SDL_PumpEvents();
        int count = 0;
        SDL_JoystickID* ids = SDL_GetGamepads(&count);
        if (ids && count > 0) {
            if (gamepad) {
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
            }
            gamepad = SDL_OpenGamepad(ids[0]);
            SDL_free(ids);
            joystickOfficiel = (gamepad != nullptr);
        } else {
            // Manette déconnectée
            if (gamepad && !SDL_GamepadConnected(gamepad)) {
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
                joystickOfficiel = false;
            }
            if (ids) {
                SDL_free(ids);
            }
        }
    } else {
        // Vérifier si la manette existante est toujours connectée
        if (!SDL_GamepadConnected(gamepad)) {
            SDL_CloseGamepad(gamepad);
            gamepad = nullptr;
            joystickOfficiel = false;
        }
    }
}

int Touches::useLastEncodeur()
{
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