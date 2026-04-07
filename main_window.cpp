#include "main_window.h"
#include "ecran_regles.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowState(Qt::WindowFullScreen);

    gestionnaireAudio = new GestionnaireAudio(this);
    touches = new Touches();

    afficherMenuPrincipal(true);
}

MainWindow::~MainWindow() {
    delete touches;
}

void MainWindow::afficherMenuPrincipal(bool restartMusique) {
    if (this->menuPrincipal == nullptr) {
        this->menuPrincipal = new MenuPrincipal(gestionnaireAudio, restartMusique, this, touches);
        connect(this->menuPrincipal, &MenuPrincipal::jouerDemande, this, &MainWindow::afficherEcranParametres);
    }

    this->ecranParametres = nullptr;
    this->ecranRegles = nullptr;
    this->ecranJeu = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->menuPrincipal);
}

void MainWindow::afficherEcranParametres()
{
    if (this->ecranParametres == nullptr) {
        this->ecranParametres = new EcranParametres(this->gestionnaireAudio, this->touches, this);

        connect(ecranParametres, &EcranParametres::demarrerPartieDemande, this, [this](ConfigurationPartie config)
        {
            derniereConfigurationPartie = config;
            aDerniereConfigurationPartie = true;
            afficherEcranRegles(config);
        });

        connect(ecranParametres, &EcranParametres::retourMenuDemande, this, [this](ConfigurationPartie config)
        {
            derniereConfigurationPartie = config;
            aDerniereConfigurationPartie = true;
            afficherMenuPrincipal(false);
        });
    }

    this->menuPrincipal = nullptr;
    this->ecranRegles = nullptr;
    this->ecranJeu = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->ecranParametres);

    if (aDerniereConfigurationPartie && ecranParametres) {
        ecranParametres->chargerConfiguration(derniereConfigurationPartie);
    }
}

void MainWindow::afficherEcranRegles(const ConfigurationPartie& configuration)
{
    if (!this->ecranRegles) {
        this->ecranRegles = new EcranRegles(this->gestionnaireAudio, this->touches, this);

        connect(ecranRegles, &EcranRegles::demarrerPartieDemande, this, [this](ConfigurationPartie config)
        {
            afficherEcranJeu(config);
        });

        connect(ecranRegles, &EcranRegles::retourParametresDemande, this, [this]()
        {
            afficherEcranParametres();
        });
    }

    ecranRegles->setConfiguration(configuration);

    this->menuPrincipal = nullptr;
    this->ecranParametres = nullptr;
    this->ecranJeu = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->ecranRegles);
}

void MainWindow::afficherEcranJeu(const ConfigurationPartie& configuration) {
    this->derniereConfigurationPartie = configuration;

    if (!this->ecranJeu) {
        this->ecranJeu = new EcranJeu(this->gestionnaireAudio, configuration, this, touches);

        connect(this->ecranJeu, &EcranJeu::finPartie, this, [this](int score) {
            afficherEcranFinPartie(score);
            });

        connect(this->ecranJeu, &EcranJeu::retourMenuDemande, this, [this]() {
            afficherMenuPrincipal(true);
        });
    }

    this->menuPrincipal = nullptr;
    this->ecranParametres = nullptr;
    this->ecranRegles = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->ecranJeu);
}

void MainWindow::afficherEcranFinPartie(int score) {
    if (!this->ecranFinPartie) {
        this->ecranFinPartie = new EcranFinPartie(this->gestionnaireAudio, this, touches);

        connect(this->ecranFinPartie, &EcranFinPartie::retourMenuDemande,
                this, [this](const QString& nomJoueur, int scoreJoueur) {
            if (!nomJoueur.trimmed().isEmpty()) {
                GestionnaireScores::instance().ajouterScore(nomJoueur.toUpper(), scoreJoueur);
            }
            afficherMenuPrincipal(true);
        });
    }

    this->ecranFinPartie->setNomParDefaut(this->derniereConfigurationPartie.nomJoueur);
    this->ecranFinPartie->setScore(score);
    this->ecranJeu = nullptr;
    this->ecranRegles = nullptr;
    this->ecranParametres = nullptr;
    this->menuPrincipal = nullptr;

    this->setCentralWidget(this->ecranFinPartie);
}
