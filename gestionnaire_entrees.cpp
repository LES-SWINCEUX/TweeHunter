#include "gestionnaire_entrees.h"

GestionnaireEntrees::GestionnaireEntrees(const ConfigurationPartie& config, Reticule* reticule, SDL_Gamepad* gamepad, QObject* parent)
    : QObject(parent)
    , config(config)
    , reticule(reticule)
    , gamepad(gamepad)
{
    if (config.manette == TypeManette::CUSTOM) {
        Touches* t = reticule ? reticule->getTouches() : nullptr;
        if (t && t->isJoystickPersoConnected()) {
            threadSerie = new QThread(this);
            lecteurSerie = new LecteurSerie(t);
            lecteurSerie->moveToThread(threadSerie);

            connect(threadSerie, &QThread::started, lecteurSerie, &LecteurSerie::demarrer);
            connect(lecteurSerie, &LecteurSerie::gachetteChangee, this, [this](bool v) { etatGachette = v; });
            connect(lecteurSerie, &LecteurSerie::reloadChange, this, [this](bool v) { etatReload = v; });
            connect(lecteurSerie, &LecteurSerie::accelerometreChange, this, [this](bool v) { etatAccelerometre = v; });
            connect(lecteurSerie, &LecteurSerie::encodeurChange, this, [this](int v) { etatEncodeur = v; });

            threadSerie->start();
        }
    }
}

GestionnaireEntrees::~GestionnaireEntrees()
{
    if (lecteurSerie) lecteurSerie->arreter();
    if (threadSerie) {
        threadSerie->quit();
        threadSerie->wait();
    }
}

void GestionnaireEntrees::lire(qint64 deltaMs)
{
    switch (config.manette) {
    case TypeManette::STANDARD:
        lireManetteStandard();
        break;
    case TypeManette::CUSTOM:
        lireManetteCustom(deltaMs);
        break;
    default:
        break;
    }
}

void GestionnaireEntrees::lireManetteStandard()
{
    if (!gamepad) {
        return;
    }

    SDL_UpdateGamepads();

    const bool gachetteTir = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 10000;
    const bool gachettePowerUp = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 10000;

    if (gachetteTir && !gachetteTirPrecedente) {
        emit tireDemande();
    }

    if (gachettePowerUp && !gachettePowerUpPrecedente) {
        emit powerUpDemande();
    }

    gachetteTirPrecedente = gachetteTir;
    gachettePowerUpPrecedente = gachettePowerUp;

    const bool reload = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST);

    if (reload && !reloadPrecedent) {
        emit reloadDemande();
    }

    reloadPrecedent = reload;

    const bool start = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);

    if (start && !startPrecedent) {
        emit pauseDemande();
    }

    startPrecedent = start;
}

void GestionnaireEntrees::lireManetteCustom(qint64 deltaMs)
{

    if (etatGachette && !etatReload) {
        if (!gachetteTirPrecedente) {
            emit tireDemande();
        }
        gachetteTirPrecedente = true;
    }
    else if (!etatGachette) {
        gachetteTirPrecedente = false;
    }

    if (etatReload && etatAccelerometre) {
        emit reloadDemande();
    }

    if (etatReload && etatEncodeur != 0) {
        etatEncodeur = 0;
        emit pauseDemande();
    }

    if (etatGachette && etatReload) {
        if (!powerUpActif) {
            powerUpActif = true;
            emit powerUpDemande();
        }
    }
    else {
        powerUpActif = false;
    }

    emit joystickDeplace(float(deltaMs));
}
