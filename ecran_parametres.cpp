#include "ecran_parametres.h"

namespace
{
    QSize tailleSource(const QSharedPointer<QPixmap>& pixmap)
    {
        if (!pixmap || pixmap->isNull()) {
            return QSize();
        }
        return pixmap->size();
    }
}

EcranParametres::EcranParametres(GestionnaireAudio* gestionnaireAudio,
    Touches* touches,
    QWidget* parent)
    : QWidget(parent),
    gestionnaireAudio(gestionnaireAudio),
    touches(touches),
    arrierePlan(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/background.png")),
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

    connect(&timerManette, &QTimer::timeout, this, &EcranParametres::tickManette);
    timerManette.setInterval(16);
    timerManette.start();
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

    boutonGrenade->setActif(configuration.powerUp ==PowerUpType::GRENADE);
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

QRect EcranParametres::calculerRectTitre(const QSharedPointer<QPixmap>& pixmap, int centreX, int y, int hauteurCible, int largeurMax) const
{
    const QSize source = tailleSource(pixmap);
    if (!source.isValid() || hauteurCible <= 0 || largeurMax <= 0) {
        return QRect();
    }

    float echelle = float(hauteurCible) / float(source.height());
    int largeur = int(source.width() * echelle);
    int hauteur = int(source.height() * echelle);

    if (largeur > largeurMax) {
        echelle = float(largeurMax) / float(source.width());
        largeur = int(source.width() * echelle);
        hauteur = int(source.height() * echelle);
    }

    return QRect(centreX - largeur / 2, y, largeur, hauteur);
}

void EcranParametres::dessinerPixmap(QPainter& painter,
    const QSharedPointer<QPixmap>& pixmap,
    const QRect& destination) const
{
    if (!pixmap || pixmap->isNull() || !destination.isValid()) {
        return;
    }

    painter.drawPixmap(destination, *pixmap, pixmap->rect());
}

int EcranParametres::placerGrille(const std::vector<Bouton*>& boutons, int colonnes, int x, int y, int largeurDisponible, int hauteurBoutonCible, int espacementX, int espacementY)
{
    if (boutons.empty() || colonnes <= 0 || largeurDisponible <= 0) {
        return 0;
    }

    const QSize base = boutons.front()->tailleImage();
    if (!base.isValid()) {
        return 0;
    }

    const int celluleW = (largeurDisponible - (colonnes - 1) * espacementX) / colonnes;

    float echelle = std::min(float(celluleW) / float(base.width()),
        float(hauteurBoutonCible) / float(base.height()));
    echelle = std::clamp(echelle, 0.18f, 1.25f);

    for (Bouton* bouton : boutons) {
        if (bouton) {
            bouton->setEchelle(echelle);
        }
    }

    const int largeurBouton = boutons.front()->width();
    const int hauteurBouton = boutons.front()->height();
    const int nombreLignes = int((boutons.size() + colonnes - 1) / colonnes);

    for (int ligne = 0; ligne < nombreLignes; ++ligne) {
        const int debut = ligne * colonnes;
        const int nombreSurLigne = std::min(colonnes, int(boutons.size()) - debut);

        for (int i = 0; i < nombreSurLigne; ++i) {
            Bouton* bouton = boutons[debut + i];
            if (!bouton) {
                continue;
            }

            bouton->move(
                x + i * (largeurBouton + espacementX),
                y + ligne * (hauteurBouton + espacementY)
            );
        }
    }

    return nombreLignes * hauteurBouton + (nombreLignes - 1) * espacementY;
}

void EcranParametres::placerElements()
{
    const int W = width();
    const int H = height();
    if (W <= 0 || H <= 0) {
        return;
    }

    const float echelleH = std::clamp(float(H) / 1080.0f, 0.72f, 1.12f);
    const float echelleW = std::clamp(float(W) / 1920.0f, 0.80f, 1.20f);
    const float echelle = std::min(echelleH, echelleW);

    const int centreX = W / 2;
    const int margeX = std::max(16, int(W * 0.02f));
    const int largeurContenu = W - 2 * margeX;

    const int espacementX = std::max(10, int(14 * echelleW));
    const int espacementY = std::max(8, int(10 * echelle));
    const int espacementTitre = std::max(4, int(5 * echelle));
    const int espacementSection = std::max(10, int(12 * echelle));
    const int ecartColonnes = std::max(12, int(16 * echelleW));

    int y = std::max(8, int(10 * echelle));

    auto estimerLargeurGrille = [&](Bouton* boutonRef, int colonnes, int largeurMax, int hauteurBoutonCible, int spacingX) -> int
    {
        if (!boutonRef || colonnes <= 0 || largeurMax <= 0) {
            return 0;
        }

        const QSize base = boutonRef->tailleImage();
        if (!base.isValid()) {
            return 0;
        }

        const int celluleW = (largeurMax - (colonnes - 1) * spacingX) / colonnes;

        float scale = std::min(float(celluleW) / float(base.width()),
            float(hauteurBoutonCible) / float(base.height()));
        scale = std::clamp(scale, 0.18f, 1.25f);

        const int largeurBouton = int(base.width() * scale);
        return colonnes * largeurBouton + (colonnes - 1) * spacingX;
    };

    rectTitrePrincipal = calculerRectTitre(titrePrincipalImg, centreX, y, int(118 * echelle), int(largeurContenu * 0.52f));

    y = rectTitrePrincipal.bottom() + 1 + espacementSection;

    const int hauteurCartesHaut = int(150 * echelle);

    const int largeurArmes = estimerLargeurGrille(boutonCarabine, 3, int(largeurContenu * 0.48f), hauteurCartesHaut, espacementX);

    const int largeurPowerUps = estimerLargeurGrille(boutonGrenade, 2, int(largeurContenu * 0.28f), hauteurCartesHaut, espacementX);

    const int largeurTop = largeurArmes + ecartColonnes + largeurPowerUps;
    const int xTop = centreX - largeurTop / 2;

    const int xArmes = xTop;
    const int xPowerUps = xArmes + largeurArmes + ecartColonnes;

    appliquerDisponibiliteManettes();

    const int hauteurBanniereHaut = int(38 * echelle);

    rectTitreArmes = calculerRectTitre(titreChoixArmeImg, xArmes + largeurArmes / 2, y, hauteurBanniereHaut, int(largeurArmes * 0.52f));

    rectTitrePowerUps = calculerRectTitre(titrePowerUpsImg, xPowerUps + largeurPowerUps / 2, y, hauteurBanniereHaut, int(largeurPowerUps * 1.18f));

    const int yTop = std::max(rectTitreArmes.bottom(), rectTitrePowerUps.bottom()) + 1 + espacementTitre;

    const int hauteurArmes = placerGrille({ boutonCarabine, boutonShotgun, boutonBombardier, boutonGrpc, boutonTarte, boutonSwince }, 3, xArmes, yTop, largeurArmes, hauteurCartesHaut, espacementX, std::max(6, espacementY / 2));

    const int hauteurPowerUps = placerGrille({ boutonGrenade, boutonZap, boutonMitraillette, boutonTacticalNuke }, 2, xPowerUps, yTop, largeurPowerUps, hauteurCartesHaut, espacementX, std::max(6, espacementY / 2));

    y = yTop + std::max(hauteurArmes, hauteurPowerUps) + espacementSection;

    const int hauteurBoutonsMilieu = std::clamp(int(70 * echelle), 56, 82);

    const int largeurMode18 = std::clamp(int(165 * echelleW), 130, 220);
    const int largeurModeNormal = std::clamp(int(210 * echelleW), 160, 280);

    const int largeurDiff = std::clamp(int(180 * echelleW), 145, 240);
    const int espacementDiff = std::max(8, int(10 * echelleW));

    const int largeurModeJeu = largeurMode18 + espacementX + largeurModeNormal;
    const int largeurDifficulte = largeurDiff * 3 + espacementDiff * 2;

    const int largeurMilieu = largeurModeJeu + ecartColonnes + largeurDifficulte;
    const int xMilieu = centreX - largeurMilieu / 2;

    const int xModeJeu = xMilieu;
    const int xDifficulte = xModeJeu + largeurModeJeu + ecartColonnes;

    rectTitreModeJeu = calculerRectTitre(titreModeJeuImg, xModeJeu + largeurModeJeu / 2, y, int(36 * echelle), int(largeurModeJeu * 0.82f));

    rectTitreDifficulte = calculerRectTitre(titreDifficulteImg, xDifficulte + largeurDifficulte / 2, y, int(36 * echelle), int(largeurDifficulte * 0.62f));

    const int yMilieu = std::max(rectTitreModeJeu.bottom(), rectTitreDifficulte.bottom()) + 1 + espacementTitre;

    if (boutonMode18) {
        boutonMode18->setGeometry(xModeJeu, yMilieu, largeurMode18, hauteurBoutonsMilieu);
    }
    if (boutonModeNormal) {
        boutonModeNormal->setGeometry(xModeJeu + largeurMode18 + espacementX, yMilieu, largeurModeNormal, hauteurBoutonsMilieu);
    }

    if (boutonDifficulteNormal) {
        boutonDifficulteNormal->setGeometry(xDifficulte, yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }
    if (boutonDifficulteRng) {
        boutonDifficulteRng->setGeometry(xDifficulte + largeurDiff + espacementDiff, yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }
    if (boutonDifficulteChaos) {
        boutonDifficulteChaos->setGeometry(xDifficulte + 2 * (largeurDiff + espacementDiff), yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }

    y = yMilieu + hauteurBoutonsMilieu + espacementSection;

    const int hauteurManette = int(145 * echelle);

    std::vector<Bouton*> boutonsManetteVisibles;
    if (boutonManetteStandard && boutonManetteStandard->isVisible()) {
        boutonsManetteVisibles.push_back(boutonManetteStandard);
    }
    if (boutonManetteCustom && boutonManetteCustom->isVisible()) {
        boutonsManetteVisibles.push_back(boutonManetteCustom);
    }
    if (boutonClavierSouris && boutonClavierSouris->isVisible()) {
        boutonsManetteVisibles.push_back(boutonClavierSouris);
    }

    int largeurManette = 0;
    int xManette = centreX;

    if (!boutonsManetteVisibles.empty()) {
        largeurManette = estimerLargeurGrille(boutonsManetteVisibles.front(), int(boutonsManetteVisibles.size()), int(largeurContenu * 0.44f), hauteurManette, espacementX);

        xManette = centreX - largeurManette / 2;
    }

    const int largeurBanniereManette = std::clamp(
        std::max(int(largeurManette * 0.90f), int(largeurContenu * 0.22f)),
        260,
        520
    );

    rectTitreChoixManette = calculerRectTitre(
        titreChoixManetteImg,
        centreX,
        y,
        int(38 * echelle),
        largeurBanniereManette
    );

    y = rectTitreChoixManette.bottom() + 1 + espacementTitre;

    if (!boutonsManetteVisibles.empty()) {
        y += placerGrille(boutonsManetteVisibles, int(boutonsManetteVisibles.size()), xManette, y, largeurManette, hauteurManette, espacementX, espacementY) + espacementSection;
    }

    const int largeurBlocNom = std::clamp(int(largeurContenu * 0.18f), 280, 380);

    rectTitreNom = calculerRectTitre(titreNomImg, centreX, y, int(38 * echelle), int(largeurBlocNom * 1.18f));

    y = rectTitreNom.bottom() + 1 + espacementTitre;

    rectFondNom = calculerRectTitre(fondNomImg, centreX, y, int(56 * echelle), largeurBlocNom);

    if (champNom) {
        QFont f = policeTitre;
        f.setPixelSize(std::max(12, int(rectFondNom.height() * 0.48f)));
        champNom->setFont(f);
        champNom->setGeometry(rectFondNom.adjusted(20, 8, -50, -8));
    }

    y = rectFondNom.bottom() + 1 + espacementSection;

    if (boutonRetour && boutonCommencer) {
        const QSize baseRetour = boutonRetour->tailleImage();
        const QSize baseJouer = boutonCommencer->tailleImage();

        if (baseRetour.isValid() && baseJouer.isValid()) {
            const int hauteurBoutonCible = std::clamp(int(68 * echelle), 48, 82);
            const int espaceEntreBoutons = std::max(14, int(18 * echelleW));

            float echelleRetour = float(hauteurBoutonCible) / float(baseRetour.height());
            float echelleJouer = float(hauteurBoutonCible) / float(baseJouer.height());

            echelleRetour = std::clamp(echelleRetour, 0.20f, 1.25f);
            echelleJouer = std::clamp(echelleJouer, 0.20f, 1.25f);

            boutonRetour->setEchelle(echelleRetour);
            boutonCommencer->setEchelle(echelleJouer);

            const int largeurTotale =
                boutonRetour->width() + espaceEntreBoutons + boutonCommencer->width();

            const int xDepart = centreX - largeurTotale / 2;

            boutonRetour->move(xDepart, y + (boutonCommencer->height() - boutonRetour->height()) / 2);

            boutonCommencer->move(xDepart + boutonRetour->width() + espaceEntreBoutons, y);
        }
    }

    centrerContenuVerticalement();

    aideFont_pixelSize = std::max(8, int(12 * std::clamp(float(H) / 1080.0f, 0.7f, 1.0f)));

    if (overlay) {
        overlay->setGeometry(rect());
        overlay->raise();
    }
}

void EcranParametres::centrerContenuVerticalement()
{
    int top = rectTitrePrincipal.top();

    int bottom = rectFondNom.bottom();
    if (boutonCommencer) {
        bottom = std::max(bottom, boutonCommencer->geometry().bottom());
    }
    if (boutonRetour) {
        bottom = std::max(bottom, boutonRetour->geometry().bottom());
    }

    const int hauteurContenu = bottom - top + 1;
    const int margeMin = 12;

    int decalageY = (height() - hauteurContenu) / 2 - top;

    if (top + decalageY < margeMin) {
        decalageY = margeMin - top;
    }

    if (decalageY == 0) {
        return;
    }

    rectTitrePrincipal.translate(0, decalageY);
    rectTitreArmes.translate(0, decalageY);
    rectTitrePowerUps.translate(0, decalageY);
    rectTitreModeJeu.translate(0, decalageY);
    rectTitreDifficulte.translate(0, decalageY);
    rectTitreChoixManette.translate(0, decalageY);
    rectTitreNom.translate(0, decalageY);
    rectFondNom.translate(0, decalageY);

    auto deplacerWidget = [decalageY](QWidget* w) 
    {
        if (w) {
            w->move(w->x(), w->y() + decalageY);
        }
    };

    deplacerWidget(boutonCarabine);
    deplacerWidget(boutonShotgun);
    deplacerWidget(boutonBombardier);
    deplacerWidget(boutonGrpc);
    deplacerWidget(boutonTarte);
    deplacerWidget(boutonSwince);

    deplacerWidget(boutonGrenade);
    deplacerWidget(boutonZap);
    deplacerWidget(boutonMitraillette);
    deplacerWidget(boutonTacticalNuke);

    deplacerWidget(boutonMode18);
    deplacerWidget(boutonModeNormal);

    deplacerWidget(boutonDifficulteNormal);
    deplacerWidget(boutonDifficulteRng);
    deplacerWidget(boutonDifficulteChaos);

    deplacerWidget(boutonManetteStandard);
    deplacerWidget(boutonManetteCustom);
    deplacerWidget(boutonClavierSouris);

    deplacerWidget(champNom);
    deplacerWidget(boutonRetour);
    deplacerWidget(boutonCommencer);
}

void EcranParametres::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (!arrierePlanCache.isNull()) {
        painter.drawPixmap(0, 0, arrierePlanCache);
    }

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    dessinerPixmap(painter, titrePrincipalImg, rectTitrePrincipal);
    dessinerPixmap(painter, titreChoixArmeImg, rectTitreArmes);
    dessinerPixmap(painter, titrePowerUpsImg, rectTitrePowerUps);
    dessinerPixmap(painter, titreModeJeuImg, rectTitreModeJeu);
    dessinerPixmap(painter, titreDifficulteImg, rectTitreDifficulte);
    dessinerPixmap(painter, titreChoixManetteImg, rectTitreChoixManette);
    dessinerPixmap(painter, titreNomImg, rectTitreNom);
    dessinerPixmap(painter, fondNomImg, rectFondNom);
}

void EcranParametres::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    arrierePlanCache = QPixmap();
    if (arrierePlan && !arrierePlan->isNull() && width() > 0 && height() > 0) {
        QPixmap scaled = arrierePlan->scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const int x = std::max(0, (scaled.width() - width()) / 2);
        const int y = std::max(0, (scaled.height() - height()) / 2);
        arrierePlanCache = scaled.copy(QRect(x, y, width(), height()));
    }

    placerElements();
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

QRect EcranParametres::rectNavigable(QWidget* widget) const
{
    if (!widget) {
        return QRect();
    }

    if (widget == champNom) {
        return rectFondNom;
    }

    return widget->geometry();
}

QPoint EcranParametres::centreNavigable(QWidget* widget) const
{
    return rectNavigable(widget).center();
}

int EcranParametres::trouverProchainIndexDansDirection(int dx, int dy) const
{
    if (widgetsNavigables.empty()) {
        return -1;
    }

    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        for (int i = 0; i < int(widgetsNavigables.size()); ++i) {
            QWidget* w = widgetsNavigables[i];
            if (w && w->isVisible() && w->isEnabled()) {
                return i;
            }
        }
        return -1;
    }

    QWidget* courant = widgetsNavigables[indexFocus];
    if (!courant) {
        return -1;
    }

    const QPoint centreCourant = centreNavigable(courant);

    int meilleurIndex = -1;
    double meilleurScore = std::numeric_limits<double>::max();

    for (int i = 0; i < int(widgetsNavigables.size()); ++i) {
        if (i == indexFocus) {
            continue;
        }

        QWidget* candidat = widgetsNavigables[i];
        if (!candidat || !candidat->isVisible() || !candidat->isEnabled()) {
            continue;
        }

        const QPoint centreCandidat = centreNavigable(candidat);
        const int vx = centreCandidat.x() - centreCourant.x();
        const int vy = centreCandidat.y() - centreCourant.y();

        int primaire = 0;
        int secondaire = 0;

        if (dx != 0) {
            primaire = vx * dx;
            secondaire = std::abs(vy);
        }
        else {
            primaire = vy * dy;
            secondaire = std::abs(vx);
        }

        if (primaire <= 0) {
            continue;
        }

        double score = double(primaire) + double(secondaire) * 2.75;

        if (secondaire < 24) {
            score *= 0.75;
        }

        if (score < meilleurScore) {
            meilleurScore = score;
            meilleurIndex = i;
        }
    }

    return meilleurIndex;
}

void EcranParametres::deplacerFocusDirection(int dx, int dy)
{
    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        const int premier = trouverProchainIndexDansDirection(dx, dy);
        if (premier >= 0) {
            appliquerFocus(premier);
        }
        return;
    }

    QWidget* courant = widgetsNavigables[indexFocus];
    if (!courant) {
        return;
    }

    auto focusWidget = [this](QWidget* cible) -> bool
    {
        if (!cible || !cible->isVisible() || !cible->isEnabled()) {
            return false;
        }

        const auto it = std::find(widgetsNavigables.begin(), widgetsNavigables.end(), cible);
        if (it == widgetsNavigables.end()) {
            return false;
        }

        appliquerFocus(int(std::distance(widgetsNavigables.begin(), it)));
        return true;
    };

    auto focusNom = [this, &focusWidget]() -> bool
    {
        return focusWidget(champNom);
    };

    auto manettesVisibles = [this]() -> std::vector<QWidget*>
    {
        std::vector<QWidget*> resultat;

        if (boutonManetteStandard && boutonManetteStandard->isVisible() && boutonManetteStandard->isEnabled()) {
            resultat.push_back(boutonManetteStandard);
        }
        if (boutonManetteCustom && boutonManetteCustom->isVisible() && boutonManetteCustom->isEnabled()) {
            resultat.push_back(boutonManetteCustom);
        }
        if (boutonClavierSouris && boutonClavierSouris->isVisible() && boutonClavierSouris->isEnabled()) {
            resultat.push_back(boutonClavierSouris);
        }

        return resultat;
    };

    auto premiereManetteVisible = [&]() -> QWidget*
    {
        auto liste = manettesVisibles();
        return liste.empty() ? nullptr : liste.front();
    };

    auto deplacerDansRangee = [&](const std::vector<QWidget*>& rangee, int direction) -> bool
    {
        if (direction == 0) {
            return false;
        }

        auto it = std::find(rangee.begin(), rangee.end(), courant);
        if (it == rangee.end()) {
            return false;
        }

        const int index = int(std::distance(rangee.begin(), it));
        const int suivant = index + direction;

        if (suivant < 0 || suivant >= int(rangee.size())) {
            return true;
        }

        return focusWidget(rangee[suivant]);
    };

    const std::vector<QWidget*> rangeeMilieu = {
        boutonMode18,
        boutonModeNormal,
        boutonDifficulteNormal,
        boutonDifficulteRng,
        boutonDifficulteChaos
    };

    const std::vector<QWidget*> rangeeBoutonsBas = {
        boutonRetour,
        boutonCommencer
    };

    const auto rangeeManette = manettesVisibles();

    const bool focusSurBlocMilieu = courant == boutonMode18 || courant == boutonModeNormal || courant == boutonDifficulteNormal || courant == boutonDifficulteRng || courant == boutonDifficulteChaos;

    const bool focusSurManette = courant == boutonManetteStandard || courant == boutonManetteCustom || courant == boutonClavierSouris;

    const bool focusSurBoutonsBas = courant == boutonRetour || courant == boutonCommencer;

    if (dx != 0) {
        if (courant == champNom) {
            return;
        }

        if (deplacerDansRangee(rangeeMilieu, dx)) {
            return;
        }

        if (deplacerDansRangee(rangeeManette, dx)) {
            return;
        }

        if (deplacerDansRangee(rangeeBoutonsBas, dx)) {
            return;
        }
    }

    if (dy > 0 && focusSurBlocMilieu) {
        if (focusWidget(premiereManetteVisible())) {
            return;
        }
    }

    if (dy > 0 && focusSurManette) {
        if (focusNom()) {
            return;
        }
    }

    if (dy < 0 && focusSurBoutonsBas) {
        if (focusNom()) {
            return;
        }
    }

    if (dy < 0 && courant == champNom) {
        if (focusWidget(premiereManetteVisible())) {
            return;
        }
    }

    if (dy > 0 && courant == champNom) {
        if (focusWidget(boutonCommencer)) {
            return;
        }
    }

    const int prochain = trouverProchainIndexDansDirection(dx, dy);
    if (prochain >= 0) {
        appliquerFocus(prochain);
    }
}

void EcranParametres::gererNavigationJoystick(float axeX, float axeY)
{
    const float deadZone = 0.55f;

    if (std::abs(axeX) < deadZone && std::abs(axeY) < deadZone) {
        verrouNavigationJoystick = false;
        return;
    }

    if (verrouNavigationJoystick) {
        return;
    }

    if (std::abs(axeX) > std::abs(axeY)) {
        deplacerFocusDirection(axeX > 0.0f ? 1 : -1, 0);
    }
    else {
        deplacerFocusDirection(0, axeY > 0.0f ? 1 : -1);
    }

    verrouNavigationJoystick = true;
}

void EcranParametres::appliquerFocus(int nouvelIndex)
{
    if (nouvelIndex < 0 || nouvelIndex >= int(widgetsNavigables.size())) {
        return;
    }

    QWidget* cible = widgetsNavigables[nouvelIndex];
    if (!cible || !cible->isVisible() || !cible->isEnabled()) {
        return;
    }

    if (indexFocus >= 0 && indexFocus < int(widgetsNavigables.size())) {
        if (auto* bouton = qobject_cast<Bouton*>(widgetsNavigables[indexFocus])) {
            bouton->setSelectionneManette(false);
        }
        else if (widgetsNavigables[indexFocus] && widgetsNavigables[indexFocus]->hasFocus()) {
            widgetsNavigables[indexFocus]->clearFocus();
            widgetsNavigables[indexFocus]->update();
        }
    }

    indexFocus = nouvelIndex;

    QWidget* widget = widgetsNavigables[indexFocus];
    if (auto* bouton = qobject_cast<Bouton*>(widget)) {
        bouton->setSelectionneManette(true);
    }
    else if (widget) {
        widget->setFocus(Qt::OtherFocusReason);
        widget->update();
    }

    update();
}

void EcranParametres::confirmerFocus()
{
    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        return;
    }

    QWidget* widget = widgetsNavigables[indexFocus];

    if (auto* boutonSprite = qobject_cast<Bouton*>(widget)) {
        boutonSprite->simulerClic();
        return;
    }

    if (auto* boutonQt = qobject_cast<QPushButton*>(widget)) {
        boutonQt->click();
        return;
    }

    if (widget == champNom) {
        champNom->setFocus(Qt::OtherFocusReason);
    }
}

void EcranParametres::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    if (transitionEnCours) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
        deplacerFocusDirection(-1, 0);
        event->accept();
        return;

    case Qt::Key_Right:
        deplacerFocusDirection(1, 0);
        event->accept();
        return;

    case Qt::Key_Up:
        deplacerFocusDirection(0, -1);
        event->accept();
        return;

    case Qt::Key_Down:
        deplacerFocusDirection(0, 1);
        event->accept();
        return;

    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        confirmerFocus();
        event->accept();
        return;

    case Qt::Key_Escape:
        lancerRetourMenu();
        event->accept();
        return;

    default:
        break;
    }

    QWidget::keyPressEvent(event);
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

void EcranParametres::tickManette()
{
    if (transitionEnCours || widgetsNavigables.empty()) {
        return;
    }

    SDL_UpdateGamepads();

    bool dpadHaut = false;
    bool dpadBas = false;
    bool dpadGauche = false;
    bool dpadDroite = false;
    bool ok = false;

    float axeX = 0.0f;
    float axeY = 0.0f;

    if (touches && touches->isJoystickConnected()) {
        SDL_Gamepad* gamepad = touches->getGamepad();
        if (gamepad) {
            const Sint16 axeBrutX = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
            const Sint16 axeBrutY = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

            axeX = std::clamp(float(axeBrutX) / 32767.0f, -1.0f, 1.0f);
            axeY = std::clamp(float(axeBrutY) / 32767.0f, -1.0f, 1.0f);

            dpadHaut = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
            dpadBas = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
            dpadGauche = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
            dpadDroite = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

            ok = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)
                || SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);
        }
    }

    if (dpadHaut && !hautPrecedent) {
        deplacerFocusDirection(0, -1);
    }
    if (dpadBas && !basPrecedent) {
        deplacerFocusDirection(0, 1);
    }
    if (dpadGauche && !gauchePrecedent) {
        deplacerFocusDirection(-1, 0);
    }
    if (dpadDroite && !droitePrecedent) {
        deplacerFocusDirection(1, 0);
    }

    gererNavigationJoystick(axeX, axeY);

    if (ok && !okPrecedent) {
        confirmerFocus();
    }

    hautPrecedent = dpadHaut;
    basPrecedent = dpadBas;
    gauchePrecedent = dpadGauche;
    droitePrecedent = dpadDroite;
    okPrecedent = ok;

    if (touches && touches->isJoystickPersoConnected()) {
        touches->lirePerso();

        const float customX = std::clamp((float(touches->getxPerso()) - 512.0f) / 512.0f, -1.0f, 1.0f);
        const float customY = std::clamp((float(touches->getyPerso()) - 512.0f) / 512.0f, -1.0f, 1.0f);
        const bool customOk = touches->getGachette();

        const float deadZone = 0.35f;
        if (std::abs(customX) < deadZone && std::abs(customY) < deadZone) {
            verrouNavigationJoystickCustom = false;
        }
        else if (!verrouNavigationJoystickCustom) {
            if (std::abs(customX) > std::abs(customY)) {
                deplacerFocusDirection(customX > 0.0f ? 1 : -1, 0);
            }
            else {
                deplacerFocusDirection(0, customY > 0.0f ? 1 : -1);
            }
            verrouNavigationJoystickCustom = true;
        }

        if (customOk && !customOkPrecedent) {
            confirmerFocus();
        }

        customOkPrecedent = customOk;
    }
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