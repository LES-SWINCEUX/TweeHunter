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
            connect(serial.getReader(), &SerialReaderThread::donneesRecues,
                this, &Touches::lirePerso, Qt::DirectConnection);
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
            QMutexLocker lock(&persoMutex);
            pendingX = obj["x"].toInt();
            pendingY = 1023 - obj["y"].toInt();
            hasNewJoystick = true;
        }
        else if (obj["type"] == "event") {
            QMutexLocker lock(&persoMutex);
            gachette = (obj["btn"].toInt() == 1);
            reload = (obj["btn1"].toInt() == 1);
            accelerometre = (obj["btn2"].toInt() == 1);
            encodeur = obj["encodeur"].toInt();
            if (encodeur != 0) {
                lastEncodeur = encodeur;
            }
        }
        else if (obj["type"] == "muon") {
            if (obj["Buff_Muons"].toInt() == 1) {
                emit detectionMuon();
            }
        }
    }

    {
        QMutexLocker lock(&persoMutex);
        if (hasNewJoystick) {
            x = pendingX;
            y = pendingY;
            hasNewJoystick = false;
        }
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

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP) || axeY() < -DEAD_ZONE_STANDARD;
}

bool Touches::bas() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN) || axeY() > DEAD_ZONE_STANDARD;
}

bool Touches::gauche() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT) || axeX() < -DEAD_ZONE_STANDARD;
}

bool Touches::droite() const
{
    if (!gamepad) {
        return false;
    }

    return SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) || axeX() > DEAD_ZONE_STANDARD;
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
    return y < SEUIL_HAUT_CUSTOM;
}

bool Touches::customBas() const
{
    return y > SEUIL_BAS_CUSTOM;
}

bool Touches::customGauche() const
{
    return customAxeX() < -DEAD_ZONE_CUSTOM;
}

bool Touches::customDroite() const
{
    return customAxeX() > DEAD_ZONE_CUSTOM;
}

float Touches::customAxeX() const
{
    QMutexLocker lock(&persoMutex);
    return std::clamp((float(x) - 512.0f) / 512.0f, -1.0f, 1.0f);
}

float Touches::customAxeY() const
{
    QMutexLocker lock(&persoMutex);
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

        const bool enDeadZone = std::abs(axeX()) < DEAD_ZONE_STANDARD && std::abs(axeY()) < DEAD_ZONE_STANDARD;

        if (enDeadZone) {
            navVerrouJoystick = false;
        }

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

    if (joystickPerso) {
        int snapX, snapY;
        bool snapGachette, snapReload;
        {
            QMutexLocker lock(&persoMutex);
            snapX = x; snapY = y;
            snapGachette = gachette; snapReload = reload;
        }

        const bool ch = snapY < SEUIL_HAUT_CUSTOM;
        const bool cb = snapY > SEUIL_BAS_CUSTOM;
        const float axX = std::clamp((float(snapX) - 512.0f) / 512.0f, -1.0f, 1.0f);
        const bool cg = axX < -DEAD_ZONE_CUSTOM;
        const bool cd = axX > DEAD_ZONE_CUSTOM;
        const bool co = snapGachette;

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

            if (cd && !navCustomDroitePrecedent) {
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
        navCustomDroitePrecedent = cd;
        navCustomOkPrecedent = co;
    }
}

void Touches::lireJeu(TypeManette manette, qint64 deltaMs)
{
    mettreAJour();

    switch (manette) {
    case TypeManette::STANDARD: {
        if (!gamepad) {
            return;
        }

        const bool gachetteTir = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > SEUIL_GACHETTE_CUSTOM;
        const bool gachettePowerUp = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)  > SEUIL_GACHETTE_CUSTOM;
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

        emit joystickDeplace(axeX(), axeY(), float(deltaMs));
        break;
    }
    case TypeManette::CUSTOM: {
        if (!isJoystickPersoConnected()) {
            return;
        }

        int snapX, snapY;
        bool snapGachette, snapReload, snapAccelerometre;
        int snapEncodeur;
        {
            QMutexLocker lock(&persoMutex);
            snapX = x; snapY = y;
            snapGachette = gachette;
            snapReload = reload;
            snapAccelerometre = accelerometre;
            snapEncodeur = encodeur;
        }

        const float snapAxeX = std::clamp((float(snapX) - 512.0f) / 512.0f, -1.0f, 1.0f);
        const float snapAxeY = std::clamp((float(snapY) - 512.0f) / 512.0f, -1.0f, 1.0f);

        if (snapGachette && !snapReload) {
            if (!gachetteTirPrecedente) {
                emit tireDemande();
            }
            gachetteTirPrecedente = true;
        } else if (!snapGachette) {
            gachetteTirPrecedente = false;
        }

        if (snapReload && snapAccelerometre) {
            emit reloadDemande();
        }

        if (snapReload && snapEncodeur != 0) {
            emit pauseDemande();
        }

        if (snapGachette && snapReload) {
            if (!powerUpActif) {
                powerUpActif = true;
                emit powerUpDemande();
            }
        } else {
            powerUpActif = false;
        }

        emit joystickDeplace(snapAxeX, snapAxeY, float(deltaMs));
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
    QMutexLocker lock(&persoMutex);
    int temp = lastEncodeur;
    if (temp != 0) {
        lastEncodeur = 0;
    }
    return temp;
}

int Touches::getxPerso() const {
    QMutexLocker lock(&persoMutex);
    return x;
}

int Touches::getyPerso() const {
    QMutexLocker lock(&persoMutex);
    return y;
}

bool Touches::getGachette() const {
    QMutexLocker lock(&persoMutex);
    return gachette;
}

bool Touches::getReload() const {
    QMutexLocker lock(&persoMutex);
    return reload;
}

bool Touches::getAccelerometre() const {
    QMutexLocker lock(&persoMutex);
    return accelerometre;
}

int Touches::getEncodeur() const {
    QMutexLocker lock(&persoMutex);
    return encodeur;
}

bool Touches::customConfirmer() const {
    QMutexLocker lock(&persoMutex);
    return gachette;
}

bool Touches::customRetour() const {
    QMutexLocker lock(&persoMutex);
    return reload;
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

    if (!serial.isOpen()) {
        return;
    }

    QJsonObject obj;
    obj["type"] = "config";
    obj["Swince"] = 1;
    QByteArray msg = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    serial.write(msg);
}