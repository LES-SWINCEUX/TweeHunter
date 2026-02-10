#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowState(Qt::WindowFullScreen);

    afficherMenuPrincipal();
}

MainWindow::~MainWindow() {}

void MainWindow::afficherMenuPrincipal() {
    if (this->menuPrincipal == nullptr) {
        this->menuPrincipal = new MenuPrincipal();
        connect(menuPrincipal, &MenuPrincipal::jouerDemande, this, &MainWindow::afficherEcranJeu);
    }

    this->ecranJeu = nullptr;

    this->setCentralWidget(this->menuPrincipal);
}

void MainWindow::afficherEcranJeu() {
    if (!this->ecranJeu) {
        this->ecranJeu = new EcranJeu(this);
    }

    this->menuPrincipal = nullptr;

    this->setCentralWidget(this->ecranJeu);
}
