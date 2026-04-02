#ifndef CONFIGURATION_PARTIE_H
#define CONFIGURATION_PARTIE_H

#include <QString>
#include <QMetaType>
#include <QSet>

#include "modejeu.h"

enum class DifficultePartie {
    NORMAL,
    RNG,
    CHAOS
};

enum class TypeManette {
    STANDARD,
    CUSTOM,
    CLAVIER_SOURIS
};

enum class PowerUpType {
    GRENADE,
    ZAP,
    MITRAILLETTE,
    TACTICAL_NUKE
};

inline uint qHash(const PowerUpType& key, uint seed = 0)
{
    return ::qHash(static_cast<int>(key), seed);
}

struct ConfigurationPartie
{
    int arme = 1;
    ModeJeu modeJeu = ModeJeu::MOINS_18;
    DifficultePartie difficulte = DifficultePartie::NORMAL;
    TypeManette manette = TypeManette::STANDARD;
    QSet<PowerUpType> powerUps;
    QString nomJoueur;

    bool aPowerUp(PowerUpType powerUp) const
    {
        return powerUps.contains(powerUp);
    }
};

Q_DECLARE_METATYPE(ConfigurationPartie)

#endif
