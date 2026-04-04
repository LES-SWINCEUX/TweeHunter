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
#include <SDL3/SDL.h>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>

#include "compteur_balles.h"
#include "compteur_points.h"
#include "compteur_powerup.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "gestionnaire_entrees.h"
#include "jeu.h"
#include "Reticule.h"
#include "vie.h"
#include "Armes.h"
#include "menu_pause_overlay.h"
#include "Touches.h"
#include "configuration_partie.h"

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
    void paintEvent(QPaintEvent*)            override;
    void resizeEvent(QResizeEvent* e)        override;
    void showEvent(QShowEvent* e)            override;
    void keyPressEvent(QKeyEvent* e)         override;
    void mouseMoveEvent(QMouseEvent* event)  override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void initReticuleEtArmes();
    void initHUD();
    void initAudio();
    void initAnimations();
    void initMinuterie();

    void tick();

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
    QElapsedTimer frameTimer;
    QElapsedTimer elapsed;
    qint64 tempsJeuMs = 0;

    Jeu* jeu = nullptr;
    Armes* armes = nullptr;
    int maxBalles = 0;

    GestionnaireEntrees* gestionnaireEntrees = nullptr;
    Reticule* reticule = nullptr;
    SDL_Gamepad* gamepad  = nullptr;
    Touches* touches = nullptr;

    CompteurBalles* compteurBalles = nullptr;
    Vies* vies = nullptr;
    CompteurPoints* compteurPoints = nullptr;
    CompteurPowerUp* compteurPowerUp = nullptr;
    static constexpr int LARGEUR_MIN_BALLES = 120;
    static constexpr int LARGEUR_MAX_BALLES = 275;

    QSharedPointer<QPixmap> arrierePlan;
    QPixmap arrierePlanCache;

    GestionnaireAudio* gestionnaireAudio = nullptr;
    FadeOverlay* overlay = nullptr;
    FadeOverlay* overlayFadeOut = nullptr;
    MenuPauseOverlay* menuPause = nullptr;
    QTimer* timerManette = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QPropertyAnimation* fadeOutAnim = nullptr;
    QPropertyAnimation* fadeOutMusique = nullptr;
    // Membres pour la gestion des power-ups (manette)
    bool gachettePrecedente = false;
    bool reloadPrecedent = false;
    bool startPrecedent = false;

    QTimer* timer2 = nullptr;
    int compteur = 0;
};

#endif
