#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontDatabase>
#include <QApplication>
#include <QCoreApplication>
#include <SDL3/SDL.h>

#include "menu.h"
#include "ecran_jeu.h"
#include "ecran_fin_partie.h"
#include "ecran_parametres.h"
#include "ecran_regles.h"
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
    void afficherEcranRegles(const ConfigurationPartie& configuration);
    void afficherEcranJeu(const ConfigurationPartie& configuration);
    void afficherEcranFinPartie(int score);

private:
    MenuPrincipal* menuPrincipal = nullptr;
    EcranParametres* ecranParametres = nullptr;
    EcranRegles* ecranRegles = nullptr;
    EcranJeu* ecranJeu = nullptr;
    EcranFinPartie* ecranFinPartie = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    Touches* touches = nullptr;
    ConfigurationPartie derniereConfigurationPartie;
    bool aDerniereConfigurationPartie = false;
};
#endif
