#include "ecran_parametres.h"

EcranParametres::EcranParametres(GestionnaireAudio* gestionnaireAudio, Touches* touchesParam, QWidget* parent) :
    QWidget(parent),
    gestionnaireAudio(gestionnaireAudio),
    touches(touchesParam),
    bg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/background.png")),
    titrePrincipalImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre.png")),
    titreChoixArmeImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_choix_arme.png")),
    titreModeJeuImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_mode_jeu.png")),
    titrePowerUpsImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_choix_power_ups.png")),
    titreDifficulteImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_difficulte.png")),
    titreChoixManetteImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_choix_manette.png")),
    titreNomImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/titre_entrer_nom.png")),
    fondNomImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/parameters/entrez_nom.png"))
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);

    policeTitre.setFamily("Luckiest Guy");
    policeTitre.setBold(true);

    creerInterface();

    appliquerDisponibiliteManettes();

    chargerConfiguration(GestionnaireConfiguration::instance().charger());

    connecterSignaux();
    appliquerEtatVisuel();

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();

    fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeInAnim->setDuration(600);
    fadeInAnim->setStartValue(255);
    fadeInAnim->setEndValue(0);
    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        if (overlay) {
            overlay->hide();
        }
        });

    fadeOutAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeOutAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeOutAnim->setDuration(600);

    connect(&timerManette, &QTimer::timeout, this, [this]() {
        if (transitionEnCours || widgetsNavigables.empty()) {
            return;
        }

        if (touches) {
            const bool standardAvant = touches->isJoystickConnected();
            const bool customAvant = touches->isJoystickPersoConnected();

            touches->verifierConnexion();

            const bool standardApres = touches->isJoystickConnected();
            const bool customApres = touches->isJoystickPersoConnected();
            if (standardAvant != standardApres || customAvant != customApres) {
                appliquerDisponibiliteManettes();
                appliquerEtatVisuel();
                placerElements();

                const bool manetteDisponibleApres = standardApres || customApres;
                if (!manetteDisponibleApres) {
                    // Effacer le focus de navigation sans toucher à l'état actif des boutons
                    if (indexFocus >= 0 && indexFocus < int(widgetsNavigables.size())) {
                        if (auto* bouton = qobject_cast<Bouton*>(widgetsNavigables[indexFocus])) {
                            bouton->setSelectionneManette(false);
                        }
                        else if (widgetsNavigables[indexFocus]) {
                            widgetsNavigables[indexFocus]->clearFocus();
                            widgetsNavigables[indexFocus]->update();
                        }
                    }
                    indexFocus = -1;
                    setFocus(Qt::OtherFocusReason);
                    update();
                }
            }
            touches->lireNavigation();
        }
        });
    timerManette.setInterval(16);
    timerManette.start();

    if (touches) {
        connect(touches, &Touches::naviguerHaut, this, [this]() { deplacerFocusDirection(0, -1); });
        connect(touches, &Touches::naviguerBas, this, [this]() { deplacerFocusDirection(0, 1); });
        connect(touches, &Touches::naviguerGauche, this, [this]() { deplacerFocusDirection(-1, 0); });
        connect(touches, &Touches::naviguerDroite, this, [this]() { deplacerFocusDirection(1, 0); });
        connect(touches, &Touches::naviguerConfirmer, this, [this]() { confirmerFocus(); });
    }
}

void EcranParametres::creerInterface()
{
    boutonCarabine = new Bouton("/images/parameters/carabine.png", 3, this);
    boutonShotgun = new Bouton("/images/parameters/shotgun.png", 3, this);
    boutonBombardier = new Bouton("/images/parameters/bombardier.png", 3, this);
    boutonGrpc = new Bouton("/images/parameters/grpc.png", 3, this);
    boutonTarte = new Bouton("/images/parameters/tarte.png", 3, this);
    boutonSwince = new Bouton("/images/parameters/swince.png", 3, this);

    boutonMode18 = new BoutonOptions("18+", "", BoutonOptions::Theme::Rouge, this);
    boutonModeNormal = new BoutonOptions("NORMAL", "", BoutonOptions::Theme::Vert, this);

    boutonGrenade = new Bouton("/images/parameters/grenade.png", 3, this);
    boutonZap = new Bouton("/images/parameters/zap.png", 3, this);
    boutonMitraillette = new Bouton("/images/parameters/mitraillette.png", 3, this);
    boutonTacticalNuke = new Bouton("/images/parameters/tactical_nuke.png", 3, this);

    boutonDifficulteNormal = new BoutonOptions("NORMAL", "MODE CLASSIQUE", BoutonOptions::Theme::Bleu, this);
    boutonDifficulteRng = new BoutonOptions("RNG", "RÉTICULE ALÉATOIRE", BoutonOptions::Theme::Vert, this);
    boutonDifficulteChaos = new BoutonOptions("CHAOS", "ÉNORMÉMENT D'ENNEMIS", BoutonOptions::Theme::Rouge, this);

    boutonManetteStandard = new Bouton("/images/parameters/manette_standard.png", 3, this);
    boutonManetteCustom = new Bouton("/images/parameters/manette_custom.png", 3, this);
    boutonClavierSouris = new Bouton("/images/parameters/clavier_souris.png", 3, this);

    champNom = new QLineEdit(this);
    champNom->setMaxLength(12);
    champNom->setAlignment(Qt::AlignCenter);
    champNom->setPlaceholderText("VOTRE NOM");
    champNom->setStyleSheet(
        "QLineEdit {"
        " background: transparent;"
        " border: none;"
        " color: white;"
        " selection-background-color: rgba(255, 220, 90, 180);"
        " }"
    );

    boutonRetour = new Bouton("/images/menu/retour_spritesheet.png", 3, this);
    boutonRetour->setCursor(Qt::PointingHandCursor);

    boutonCommencer = new Bouton("/images/menu/jouer_spritesheet_horizontal.png", 3, this);
    boutonCommencer->setCursor(Qt::PointingHandCursor);

    widgetsNavigables = {
        boutonCarabine, boutonShotgun, boutonBombardier,
        boutonGrpc, boutonTarte, boutonSwince,

        boutonGrenade, boutonZap, boutonMitraillette, boutonTacticalNuke,

        boutonMode18, boutonModeNormal,
        boutonDifficulteNormal, boutonDifficulteRng, boutonDifficulteChaos,

        boutonManetteStandard, boutonManetteCustom, boutonClavierSouris,

        champNom,
        boutonRetour, boutonCommencer
    };

    configuration.arme = 1;
    configuration.modeJeu = ModeJeu::MOINS_18;
    configuration.difficulte = DifficultePartie::NORMAL;
    configuration.manette = TypeManette::STANDARD;
    configuration.powerUp = PowerUpType::GRENADE;
}

void EcranParametres::connecterSignaux()
{
    auto connecterFocus = [this](QWidget* widget) {
        const auto it = std::find(widgetsNavigables.begin(), widgetsNavigables.end(), widget);
        if (it != widgetsNavigables.end()) {
            appliquerFocus(int(std::distance(widgetsNavigables.begin(), it)));
        }
        };

    connect(boutonCarabine, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 1;
        appliquerEtatVisuel();
        connecterFocus(boutonCarabine);
        });

    connect(boutonShotgun, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 2;
        appliquerEtatVisuel();
        connecterFocus(boutonShotgun);
        });

    connect(boutonBombardier, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 4;
        appliquerEtatVisuel();
        connecterFocus(boutonBombardier);
        });

    connect(boutonGrpc, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 3;
        appliquerEtatVisuel();
        connecterFocus(boutonGrpc);
        });

    connect(boutonTarte, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 5;
        appliquerEtatVisuel();
        connecterFocus(boutonTarte);
        });

    connect(boutonSwince, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.arme = 6;
        appliquerEtatVisuel();
        connecterFocus(boutonSwince);
        });

    connect(boutonMode18, &QPushButton::clicked, this, [this, connecterFocus]() {
        configuration.modeJeu = ModeJeu::PLUS_18;
        appliquerEtatVisuel();
        connecterFocus(boutonMode18);
        });

    connect(boutonModeNormal, &QPushButton::clicked, this, [this, connecterFocus]() {
        configuration.modeJeu = ModeJeu::MOINS_18;
        appliquerEtatVisuel();
        connecterFocus(boutonModeNormal);
        });

    auto selectionnerPowerUp = [this](PowerUpType powerUp, Bouton* bouton) {
        configuration.powerUp = powerUp;
        appliquerEtatVisuel();

        const auto it = std::find(widgetsNavigables.begin(), widgetsNavigables.end(), bouton);
        if (it != widgetsNavigables.end()) {
            appliquerFocus(int(std::distance(widgetsNavigables.begin(), it)));
        }
        };

    connect(boutonGrenade, &Bouton::clicked, this, [=]() {
        selectionnerPowerUp(PowerUpType::GRENADE, boutonGrenade);
        });
    connect(boutonZap, &Bouton::clicked, this, [=]() {
        selectionnerPowerUp(PowerUpType::ZAP, boutonZap);
        });
    connect(boutonMitraillette, &Bouton::clicked, this, [=]() {
        selectionnerPowerUp(PowerUpType::MITRAILLETTE, boutonMitraillette);
        });
    connect(boutonTacticalNuke, &Bouton::clicked, this, [=]() {
        selectionnerPowerUp(PowerUpType::TACTICAL_NUKE, boutonTacticalNuke);
        });

    connect(boutonDifficulteNormal, &QPushButton::clicked, this, [this, connecterFocus]() {
        configuration.difficulte = DifficultePartie::NORMAL;
        appliquerEtatVisuel();
        connecterFocus(boutonDifficulteNormal);
        });

    connect(boutonDifficulteRng, &QPushButton::clicked, this, [this, connecterFocus]() {
        configuration.difficulte = DifficultePartie::RNG;
        appliquerEtatVisuel();
        connecterFocus(boutonDifficulteRng);
        });

    connect(boutonDifficulteChaos, &QPushButton::clicked, this, [this, connecterFocus]() {
        configuration.difficulte = DifficultePartie::CHAOS;
        appliquerEtatVisuel();
        connecterFocus(boutonDifficulteChaos);
        });

    connect(boutonManetteStandard, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.manette = TypeManette::STANDARD;
        appliquerEtatVisuel();
        connecterFocus(boutonManetteStandard);
        });

    connect(boutonManetteCustom, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.manette = TypeManette::CUSTOM;
        appliquerEtatVisuel();
        connecterFocus(boutonManetteCustom);
        });

    connect(boutonClavierSouris, &Bouton::clicked, this, [this, connecterFocus]() {
        configuration.manette = TypeManette::CLAVIER_SOURIS;
        appliquerEtatVisuel();
        connecterFocus(boutonClavierSouris);
        });

    connect(champNom, &QLineEdit::selectionChanged, this, [this, connecterFocus]() {
        connecterFocus(champNom);
        });

    connect(champNom, &QLineEdit::textChanged, this, [this](const QString& texte) {
        const QString majuscule = texte.toUpper();
        if (majuscule == texte) {
            return;
        }

        QSignalBlocker bloqueur(champNom);
        champNom->setText(majuscule);
        });

    connect(champNom, &QLineEdit::returnPressed, this, &EcranParametres::lancerDemarrage);
    connect(boutonRetour, &Bouton::clicked, this, &EcranParametres::lancerRetourMenu);
    connect(boutonCommencer, &Bouton::clicked, this, &EcranParametres::lancerDemarrage);
}

void EcranParametres::appliquerEtatVisuel()
{
    appliquerDisponibiliteManettes();

    boutonCarabine->setActif(configuration.arme == 1);
    boutonShotgun->setActif(configuration.arme == 2);
    boutonBombardier->setActif(configuration.arme == 4);
    boutonGrpc->setActif(configuration.arme == 3);
    boutonTarte->setActif(configuration.arme == 5);
    boutonSwince->setActif(configuration.arme == 6);

    boutonGrenade->setActif(configuration.powerUp == PowerUpType::GRENADE);
    boutonZap->setActif(configuration.powerUp == PowerUpType::ZAP);
    boutonMitraillette->setActif(configuration.powerUp == PowerUpType::MITRAILLETTE);
    boutonTacticalNuke->setActif(configuration.powerUp == PowerUpType::TACTICAL_NUKE);

    boutonMode18->setSelected(configuration.modeJeu == ModeJeu::PLUS_18);
    boutonModeNormal->setSelected(configuration.modeJeu == ModeJeu::MOINS_18);

    boutonDifficulteNormal->setSelected(configuration.difficulte == DifficultePartie::NORMAL);
    boutonDifficulteRng->setSelected(configuration.difficulte == DifficultePartie::RNG);
    boutonDifficulteChaos->setSelected(configuration.difficulte == DifficultePartie::CHAOS);

    boutonManetteStandard->setActif(configuration.manette == TypeManette::STANDARD);
    boutonManetteCustom->setActif(configuration.manette == TypeManette::CUSTOM);
    boutonClavierSouris->setActif(configuration.manette == TypeManette::CLAVIER_SOURIS);
}

void EcranParametres::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    transitionEnCours = false;
    setEnabled(true);

    verrouNavigationJoystick = false;
    verrouNavigationJoystickCustom = false;

    appliquerDisponibiliteManettes();
    appliquerEtatVisuel();
    placerElements();

    const bool manetteDisponible = (touches && touches->isJoystickConnected()) || (touches && touches->isJoystickPersoConnected());

    if (manetteDisponible) {
        appliquerFocus(0);
    }
    else {
        if (indexFocus >= 0 && indexFocus < int(widgetsNavigables.size())) {
            if (auto* bouton = qobject_cast<Bouton*>(widgetsNavigables[indexFocus])) {
                bouton->setSelectionneManette(false);
            }
            else if (widgetsNavigables[indexFocus]) {
                widgetsNavigables[indexFocus]->clearFocus();
                widgetsNavigables[indexFocus]->update();
            }
        }

        indexFocus = -1;
        setFocus(Qt::OtherFocusReason);
        update();
    }

    if (overlay && fadeInAnim) {
        overlay->setGeometry(rect());
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();
        fadeInAnim->stop();
        fadeInAnim->start();
    }
}

ConfigurationPartie EcranParametres::configurationFinale() const
{
    ConfigurationPartie config = configuration;
    if (champNom) {
        config.nomJoueur = champNom->text().trimmed().toUpper();
    }
    return config;
}

void EcranParametres::lancerDemarrage()
{
    if (transitionEnCours) {
        return;
    }

    transitionEnCours = true;
    setEnabled(false);

    if (overlay) {
        overlay->setAlpha(0);
        overlay->show();
        overlay->raise();
    }

    if (fadeOutAnim) {
        fadeOutAnim->stop();
        fadeOutAnim->setStartValue(0);
        fadeOutAnim->setEndValue(255);
    }

    QObject::disconnect(fadeOutAnim, nullptr, this, nullptr);
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this]() {
        GestionnaireConfiguration::instance().sauvegarder(configurationFinale());
        emit demarrerPartieDemande(configurationFinale());
        });

    fadeOutAnim->start();
}

void EcranParametres::lancerRetourMenu()
{
    if (transitionEnCours) {
        return;
    }

    transitionEnCours = true;
    setEnabled(false);

    if (overlay) {
        overlay->setAlpha(0);
        overlay->show();
        overlay->raise();
    }

    if (fadeOutAnim) {
        fadeOutAnim->stop();
        fadeOutAnim->setStartValue(0);
        fadeOutAnim->setEndValue(255);
    }

    QObject::disconnect(fadeOutAnim, nullptr, this, nullptr);
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this]() {
        GestionnaireConfiguration::instance().sauvegarder(configurationFinale());
        emit retourMenuDemande(configurationFinale());
        });

    fadeOutAnim->start();
}

void EcranParametres::chargerConfiguration(const ConfigurationPartie& config)
{
    configuration = config;

    const bool standardDisponible = touches && touches->isJoystickConnected();
    const bool customDisponible = touches && touches->isJoystickPersoConnected();

    bool manetteCorrigee = false;

    if (configuration.manette == TypeManette::STANDARD && !standardDisponible) {
        configuration.manette = premiereManetteDisponible();
        manetteCorrigee = true;
    }

    if (configuration.manette == TypeManette::CUSTOM && !customDisponible) {
        configuration.manette = premiereManetteDisponible();
        manetteCorrigee = true;
    }

    if (champNom) {
        champNom->setText(configuration.nomJoueur.toUpper());
    }

    appliquerDisponibiliteManettes();
    appliquerEtatVisuel();
    update();

    if (manetteCorrigee) {
        GestionnaireConfiguration::instance().sauvegarderManette(configuration.manette);
    }
}

TypeManette EcranParametres::premiereManetteDisponible() const
{
    if (touches && touches->isJoystickConnected()) {
        return TypeManette::STANDARD;
    }

    if (touches && touches->isJoystickPersoConnected()) {
        return TypeManette::CUSTOM;
    }

    return TypeManette::CLAVIER_SOURIS;
}

void EcranParametres::appliquerDisponibiliteManettes()
{
    const bool standardDisponible = touches && touches->isJoystickConnected();
    const bool customDisponible = touches && touches->isJoystickPersoConnected();

    if (boutonManetteStandard) {
        boutonManetteStandard->setVisible(standardDisponible);
        boutonManetteStandard->setEnabled(standardDisponible);
    }

    if (boutonManetteCustom) {
        boutonManetteCustom->setVisible(customDisponible);
        boutonManetteCustom->setEnabled(customDisponible);
    }

    if (boutonClavierSouris) {
        boutonClavierSouris->setVisible(true);
        boutonClavierSouris->setEnabled(true);
    }

    bool manetteCorrigee = false;

    if (configuration.manette == TypeManette::STANDARD && !standardDisponible) {
        configuration.manette = premiereManetteDisponible();
        manetteCorrigee = true;
    }

    if (configuration.manette == TypeManette::CUSTOM && !customDisponible) {
        configuration.manette = premiereManetteDisponible();
        manetteCorrigee = true;
    }

    if (manetteCorrigee) {
        GestionnaireConfiguration::instance().sauvegarderManette(configuration.manette);
    }
}