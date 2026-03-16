#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "menu.h"
#include "ecran_jeu.h"
#include "ecran_fin_partie.h"
#include "gestionnaire_audio.h"
#include "Touches.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void afficherMenuPrincipal();
    void afficherEcranJeu();
    void afficherEcranFinPartie(int score);

private:
    MenuPrincipal*   menuPrincipal   = nullptr;
    EcranJeu*        ecranJeu        = nullptr;
    EcranFinPartie*  ecranFinPartie  = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    Touches* touches = nullptr;
};
#endif
