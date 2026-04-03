#include "gestionnaire_entrees.h"

GestionnaireEntrees::GestionnaireEntrees(const ConfigurationPartie& config, Reticule* reticule, SDL_Gamepad* gamepad, QObject* parent)
    : QObject(parent)
    , config(config)
    , reticule(reticule)
    , gamepad(gamepad)
{}

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

    const bool gachette = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 10000;

    if (gachette && !gachettePrecedente) {
        emit tireDemande();
    }

    gachettePrecedente = gachette;

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
    Touches* t = reticule ? reticule->getTouches() : nullptr;

    if (!t || !t->isJoystickPersoConnected()) {
        return;
    }

    t->lirePerso();

    const bool gachette = t->getGachette();
    const bool reload = t->getReload();

    if (gachette && !reload) {
        if (!gachettePrecedente) {
            emit tireDemande();
        }

        gachettePrecedente = true;
    } else if (!gachette) {
        gachettePrecedente = false;
    }

    if (reload && t->getAccelerometre()) {
        emit reloadDemande();
    }
        
    if (reload && t->getEncodeur() != 0) {
        emit pauseDemande();
    }

    if (gachette && reload) {
        if (!powerUpActif) { 
            powerUpActif = true;
            emit powerUpDemande(); 
        }
    } else {
        powerUpActif = false;
    }

    emit joystickDeplace(float(deltaMs));
}
