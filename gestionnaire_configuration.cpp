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
    s.setValue("powerUp", int(config.powerUp));

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
    config.powerUp = PowerUpType::GRENADE;

    s.beginGroup("config");

    config.arme = s.value("arme", config.arme).toInt();
    config.modeJeu = ModeJeu(s.value("modeJeu", int(config.modeJeu)).toInt());
    config.difficulte = DifficultePartie(s.value("difficulte", int(config.difficulte)).toInt());
    config.manette = TypeManette(s.value("manette", int(config.manette)).toInt());
    config.nomJoueur = s.value("nomJoueur", config.nomJoueur).toString();
    config.powerUp = PowerUpType(s.value("powerUp", int(config.powerUp)).toInt());

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

void GestionnaireConfiguration::sauvegarderManette(TypeManette manette)
{
    QSettings s("TweeHunter", "ConfigPartie");
    s.beginGroup("config");
    s.setValue("manette", int(manette));
    s.endGroup();
}