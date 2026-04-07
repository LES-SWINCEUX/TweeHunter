#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "menu.h"
#include "ecran_jeu.h"
#include "ecran_fin_partie.h"
#include "ecran_parametres.h"
#include "gestionnaire_audio.h"
#include "Touches.h"
#include "configuration_partie.h"
#include "gestionnaire_scores.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void afficherMenuPrincipal(bool restartMusique);
    void afficherEcranParametres();
    void afficherEcranJeu(const ConfigurationPartie& configuration);
    void afficherEcranFinPartie(int score);

private:
    MenuPrincipal* menuPrincipal = nullptr;
    EcranParametres* ecranParametres = nullptr;
    EcranJeu* ecranJeu = nullptr;
    EcranFinPartie* ecranFinPartie = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    Touches* touches = nullptr;
    ConfigurationPartie derniereConfigurationPartie;
    bool aDerniereConfigurationPartie = false;
};
#endif
