#ifndef GESTIONNAIRE_CONFIGURATION_H
#define GESTIONNAIRE_CONFIGURATION_H

#include <QSettings>
#include "configuration_partie.h"

class GestionnaireConfiguration
{
public:
    static GestionnaireConfiguration& instance()
    {
        static GestionnaireConfiguration inst;
        return inst;
    }

    void sauvegarder(const ConfigurationPartie& config);
    void sauvegarderManette(TypeManette manette);
    ConfigurationPartie charger() const;
    void reset();

private:
    GestionnaireConfiguration() = default;
    ~GestionnaireConfiguration() = default;

    GestionnaireConfiguration(const GestionnaireConfiguration&) = delete;
    GestionnaireConfiguration& operator=(const GestionnaireConfiguration&) = delete;
};

#endif