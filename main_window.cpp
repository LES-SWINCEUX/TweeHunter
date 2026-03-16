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
        this->ecranJeu = new EcranJeu(this->gestionnaireAudio, this, 5);     //Le chiffre à la fin peut être changé pour passer d'un arme à l'autre

        connect(this->ecranJeu, &EcranJeu::finPartie, this, [this](int score) {
            afficherEcranFinPartie(score);
            });

        connect(this->ecranJeu, &EcranJeu::retourMenuDemande, this, [this]() {
            afficherMenuPrincipal();
            });
        qDebug() << "serialPort avant if:" << serialPort;

        if (!serialPort) {
            serialPort = new NativeSerialPort();
            QString port = NativeSerialPort::findArduinoPort();
            qDebug() << "Port trouve:" << port;
            if (!port.isEmpty()) {
                serialPort->setPortName(port);
                serialPort->setBaudRate(115200);
                bool ok = serialPort->open(NativeSerialPort::ReadWrite);
                qDebug() << "Port ouvert:" << ok;
            }
            else {
                qDebug() << "Aucun port Arduino trouve";
            }
        }
        connect(this->ecranJeu, &EcranJeu::ballesChanged, this, [this](int nb) {
            qDebug() << "ballesChanged recu dans MainWindow, nb:" << nb;
            if (!serialPort || !serialPort->isOpen()) {
                qDebug() << "serialPort non ouvert!";
                return;
            }
            QJsonObject obj;
            obj["type"] = "config";
            obj["nb_balles"] = nb;
            QJsonDocument doc(obj);
            serialPort->write(doc.toJson(QJsonDocument::Compact) + "\n");
            qDebug() << "Envoye a Arduino:" << doc.toJson(QJsonDocument::Compact);
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
                GestionnaireScores::instance().ajouterScore(nomJoueur.toUpper(), scoreJoueur);
            }
            afficherMenuPrincipal();
        });
    }

    this->ecranFinPartie->setScore(score);
    this->ecranJeu       = nullptr;
    this->menuPrincipal  = nullptr;

    this->setCentralWidget(this->ecranFinPartie);
}

