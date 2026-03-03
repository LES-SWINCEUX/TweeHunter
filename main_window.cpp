#include "main_window.h"
#include "gestionnaire_scores.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowState(Qt::WindowFullScreen);

    gestionnaireAudio = new GestionnaireAudio(this);

    afficherMenuPrincipal();
}

MainWindow::~MainWindow() {}

void MainWindow::afficherMenuPrincipal() {
    if (this->menuPrincipal == nullptr) {
        this->menuPrincipal = new MenuPrincipal(gestionnaireAudio, this);
        connect(this->menuPrincipal, &MenuPrincipal::jouerDemande, this, &MainWindow::afficherEcranJeu);
    }

    this->ecranJeu       = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->menuPrincipal);
}

void MainWindow::afficherEcranJeu() {
    if (!this->ecranJeu) {
        this->ecranJeu = new EcranJeu(this->gestionnaireAudio, this);

        connect(this->ecranJeu, &EcranJeu::finPartie, this, [this](int score) {
            afficherEcranFinPartie(score);
        });
    }

    this->menuPrincipal  = nullptr;
    this->ecranFinPartie = nullptr;

    this->setCentralWidget(this->ecranJeu);
}

void MainWindow::afficherEcranFinPartie(int score) {
    if (!this->ecranFinPartie) {
        this->ecranFinPartie = new EcranFinPartie(this->gestionnaireAudio, this);

        connect(this->ecranFinPartie, &EcranFinPartie::retourMenuDemande,
                this, [this](const QString& nomJoueur, int scoreJoueur) {
            if (!nomJoueur.trimmed().isEmpty()) {
                GestionnaireScores::instance().ajouterScore(nomJoueur.toUpper(), 999999);
            }
            afficherMenuPrincipal();
        });
    }

    this->ecranFinPartie->setScore(score);
    this->ecranJeu       = nullptr;
    this->menuPrincipal  = nullptr;

    this->setCentralWidget(this->ecranFinPartie);
}

