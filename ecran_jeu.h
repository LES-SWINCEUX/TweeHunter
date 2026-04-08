#ifndef ECRAN_JEU_H
#define ECRAN_JEU_H

#include <QDir>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPropertyAnimation>
#include <QShowEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>
#include <random>
#include <iostream>

#include "compteur_balles.h"
#include "compteur_points.h"
#include "compteur_powerup.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "jeu.h"
#include "Reticule.h"
#include "compteur_vies.h"
#include "Armes.h"
#include "menu_pause_overlay.h"
#include "Touches.h"
#include "configuration_partie.h"
#include "arriere_plan.h"

class EcranJeu : public QWidget
{
    Q_OBJECT

public:
    EcranJeu(GestionnaireAudio* gestionnaireAudio, const ConfigurationPartie& config, QWidget* parent  = nullptr, Touches* touches = nullptr);
    ~EcranJeu();

    void tire();

signals:
    void finPartie(int score);
    void retourMenuDemande();
    void ballesChanged(int nbBalles);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void initReticuleEtArmes();
    void initHUD();
    void initAudio();
    void initAnimations();
    void initMinuterie();

    void tick();
    void timeoutReticuleAleatoire();

    void Power();
    void rechargerArme();

    void placerElementsGUI();

    void mettreEnPause();
    void reprendreJeu();
    void demarrerFadeOutVersMenu();
    void declencherFinPartie();

    void TestHitbox(QPainter& painter);

    ConfigurationPartie configurationPartie;
    bool enPause = false;
    bool transitionVersMenu = false;

    QTimer timer;
    QTimer timerReticulesAleatoire;
    QElapsedTimer frameTimer;
    QElapsedTimer elapsed;
    qint64 tempsJeuMs = 0;

    Jeu* jeu = nullptr;
    Armes* armes = nullptr;
    int maxBalles = 0;

    Reticule* reticule = nullptr;
    Touches* touches = nullptr;

    CompteurBalles* compteurBalles = nullptr;
    CompteurVies* compteurVies = nullptr;
    CompteurPoints* compteurPoints = nullptr;
    CompteurPowerUp* compteurPowerUp = nullptr;
    int LARGEUR_MIN_BALLES = 120;
    int LARGEUR_MAX_BALLES = 275;

    ArrierePlan bg;

    GestionnaireAudio* gestionnaireAudio = nullptr;
    FadeOverlay* overlay = nullptr;
    FadeOverlay* overlayFadeOut = nullptr;
    MenuPauseOverlay* menuPause = nullptr;
    QTimer* timerManette = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QPropertyAnimation* fadeOutAnim = nullptr;
    QPropertyAnimation* fadeOutMusique = nullptr;

    bool gachettePrecedente = false;
    bool reloadPrecedent = false;
    bool startPrecedent = false;

    QTimer* timer2 = nullptr;
    int compteur = 0;
};

#endif
