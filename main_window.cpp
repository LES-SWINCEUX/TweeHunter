#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowState(Qt::WindowFullScreen);

    afficherMenuPrincipal();
}

MainWindow::~MainWindow() {
    delete menuPrincipal;
}

void MainWindow::afficherMenuPrincipal() {
    if (this->menuPrincipal == nullptr) {
        this->menuPrincipal = new MenuPrincipal(this);
    }

    this->setCentralWidget(this->menuPrincipal);
}

void MainWindow::afficherEcranJeu() {}
