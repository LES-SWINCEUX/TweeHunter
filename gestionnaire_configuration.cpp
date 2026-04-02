#include "gestionnaire_configuration.h"

void GestionnaireConfiguration::sauvegarder(const ConfigurationPartie& config)
{
    QSettings s("TweeHunter", "ConfigPartie");

    s.beginGroup("config");

    s.setValue("arme", config.arme);
    s.setValue("modeJeu", int(config.modeJeu));
    s.setValue("difficulte", int(config.difficulte));
    s.setValue("manette", int(config.manette));
    s.setValue("nomJoueur", config.nomJoueur);

    s.beginWriteArray("powerUps");
    int index = 0;
    for (PowerUpType powerUp : config.powerUps) {
        s.setArrayIndex(index++);
        s.setValue("value", int(powerUp));
    }
    s.endArray();

    s.endGroup();
}

ConfigurationPartie GestionnaireConfiguration::charger() const
{
    QSettings s("TweeHunter", "ConfigPartie");

    ConfigurationPartie config;
    config.arme = 1;
    config.modeJeu = ModeJeu::MOINS_18;
    config.difficulte = DifficultePartie::NORMAL;
    config.manette = TypeManette::STANDARD;
    config.nomJoueur = "";
    config.powerUps.clear();
    config.powerUps.insert(PowerUpType::GRENADE);

    s.beginGroup("config");

    config.arme = s.value("arme", config.arme).toInt();
    config.modeJeu = ModeJeu(s.value("modeJeu", int(config.modeJeu)).toInt());
    config.difficulte = DifficultePartie(s.value("difficulte", int(config.difficulte)).toInt());
    config.manette = TypeManette(s.value("manette", int(config.manette)).toInt());
    config.nomJoueur = s.value("nomJoueur", config.nomJoueur).toString();

    config.powerUps.clear();
    int n = s.beginReadArray("powerUps");
    for (int i = 0; i < n; ++i) {
        s.setArrayIndex(i);
        config.powerUps.insert(PowerUpType(s.value("value").toInt()));
    }
    s.endArray();

    if (config.powerUps.isEmpty()) {
        config.powerUps.insert(PowerUpType::GRENADE);
    }

    s.endGroup();

    return config;
}

void GestionnaireConfiguration::reset()
{
    QSettings s("TweeHunter", "ConfigPartie");
    s.beginGroup("config");
    s.remove("");
    s.endGroup();
}