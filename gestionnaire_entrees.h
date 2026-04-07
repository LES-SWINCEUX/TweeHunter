#ifndef GESTIONNAIRE_ENTREES_H
#define GESTIONNAIRE_ENTREES_H

#include <QObject>
#include <SDL3/SDL.h>
#include "configuration_partie.h"
#include "Reticule.h"
#include "Touches.h"

class GestionnaireEntrees : public QObject
{
    Q_OBJECT

public:
    GestionnaireEntrees(const ConfigurationPartie& config, Reticule* reticule, SDL_Gamepad* gamepad, QObject* parent = nullptr);

    void lire(qint64 deltaMs);

signals:
    void tireDemande();
    void reloadDemande();
    void pauseDemande();
    void powerUpDemande();
    void joystickDeplace(float deltaMs);

private:
    void lireManetteStandard();
    void lireManetteCustom(qint64 deltaMs);

    ConfigurationPartie config;
    Reticule* reticule = nullptr;
    SDL_Gamepad* gamepad = nullptr;

    bool gachetteTirPrecedente = false;
    bool gachettePowerUpPrecedente = false;
    bool reloadPrecedent = false;
    bool startPrecedent = false;
    bool powerUpActif = false;
};

#endif
