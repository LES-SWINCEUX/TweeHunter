#ifndef GESTIONNAIRE_ENTREES_H
#define GESTIONNAIRE_ENTREES_H

#include <QObject>
#include <SDL3/SDL.h>
#include "configuration_partie.h"

class Reticule;

// ---------------------------------------------------------------------------
// GestionnaireEntrees
//
// Centralise la lecture des entrées joueur pour les trois modes de contrôle :
//   - CLAVIER_SOURIS : géré directement par les events Qt dans EcranJeu,
//                      cette classe n'intervient pas.
//   - STANDARD       : manette SDL (gâchette droite, bouton ouest, start).
//   - CUSTOM         : joystick maison via Touches (gâchette, reload,
//                      accéléromètre, encodeur).
//
// Émet des signaux à chaque action détectée ; EcranJeu s'y connecte et
// délègue le travail à ses propres méthodes.  Les flags anti-répétition
// (gachettePrecedente, etc.) vivent ici et non plus dans EcranJeu.
// ---------------------------------------------------------------------------
class GestionnaireEntrees : public QObject
{
    Q_OBJECT

public:
    GestionnaireEntrees(const ConfigurationPartie& config,
                        Reticule*    reticule,
                        SDL_Gamepad* gamepad,
                        QObject*     parent = nullptr);

    // À appeler une fois par tick (hors pause).
    // deltaMs sert uniquement au déplacement du joystick custom.
    void lire(qint64 deltaMs);

signals:
    void tireDemande();
    void reloadDemande();
    void pauseDemande();
    void powerUpDemande();
    // Émis en mode CUSTOM pour que EcranJeu appelle applyJoystickPerso().
    void joystickDeplace(float deltaMs);

private:
    void lireManetteStandard();
    void lireManetteCustom(qint64 deltaMs);

    ConfigurationPartie config;
    Reticule*    reticule = nullptr;
    SDL_Gamepad* gamepad  = nullptr;

    // Flags anti-répétition (un par bouton physique surveillé)
    bool gachettePrecedente = false;
    bool reloadPrecedent    = false;
    bool startPrecedent     = false;
    bool powerUpActif       = false;
};

#endif // GESTIONNAIRE_ENTREES_H
