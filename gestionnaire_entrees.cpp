#include "gestionnaire_entrees.h"
#include "Reticule.h"
#include "Touches.h"

GestionnaireEntrees::GestionnaireEntrees(const ConfigurationPartie& config,
                                         Reticule*    reticule,
                                         SDL_Gamepad* gamepad,
                                         QObject*     parent)
    : QObject(parent)
    , config(config)
    , reticule(reticule)
    , gamepad(gamepad)
{}

// ---------------------------------------------------------------------------
// Point d'entrée unique appelé par EcranJeu::tick()
// ---------------------------------------------------------------------------
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
        // CLAVIER_SOURIS : géré par les events Qt (mousePressEvent, keyPressEvent)
        break;
    }
}

// ---------------------------------------------------------------------------
// Manette SDL standard
// ---------------------------------------------------------------------------
void GestionnaireEntrees::lireManetteStandard()
{
    if (!gamepad) return;
    SDL_UpdateGamepads();

    // Gâchette droite → tir (front montant uniquement)
    const bool gachette = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 10000;
    if (gachette && !gachettePrecedente) emit tireDemande();
    gachettePrecedente = gachette;

    // Bouton ouest → rechargement (front montant)
    const bool reload = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST);
    if (reload && !reloadPrecedent) emit reloadDemande();
    reloadPrecedent = reload;

    // Start → pause (front montant)
    const bool start = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);
    if (start && !startPrecedent) emit pauseDemande();
    startPrecedent = start;
}

// ---------------------------------------------------------------------------
// Joystick custom (Touches)
// ---------------------------------------------------------------------------
void GestionnaireEntrees::lireManetteCustom(qint64 deltaMs)
{
    Touches* t = reticule ? reticule->getTouches() : nullptr;
    if (!t || !t->isJoystickPersoConnected()) return;

    t->lirePerso();

    const bool gachette = t->getGachette();
    const bool reload   = t->getReload();

    // Gâchette seule → tir (front montant)
    if (gachette && !reload) {
        if (!gachettePrecedente) emit tireDemande();
        gachettePrecedente = true;
    } else if (!gachette) {
        gachettePrecedente = false;
    }

    // Reload + accéléromètre → rechargement
    if (reload && t->getAccelerometre())
        emit reloadDemande();

    // Reload + encodeur → pause
    if (reload && t->getEncodeur() != 0)
        emit pauseDemande();

    // Gâchette + reload simultanés → power-up (front montant)
    if (gachette && reload) {
        if (!powerUpActif) { powerUpActif = true; emit powerUpDemande(); }
    } else {
        powerUpActif = false;
    }

    // Déplacement du réticule via joystick
    emit joystickDeplace(float(deltaMs));
}
