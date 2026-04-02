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
#include <QTimer>
#include <QDebug>
#include <cmath>

#include "compteur_balles.h"
#include "compteur_points.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "jeu.h"
#include "Reticule.h"
#include "vie.h"
#include "Armes.h"
#include "menu_pause_overlay.h"
#include "Touches.h"

class EcranJeu : public QWidget
{
    Q_OBJECT

public:
    EcranJeu(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr, int arme = 1, Touches* t=nullptr);
    ~EcranJeu();
    void tire(int typeTire);

signals:
    void finPartie(int score);
    void retourMenuDemande();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;


private:
    void tick();
    void placerElementsGUI();
    void rechargerArme();

    void mettreEnPause();
    void reprendreJeu();
    void demarrerFadeOutVersMenu();

    QTimer timer;
    QElapsedTimer elapsed;
    QElapsedTimer frameTimer;
    qint64 tempsJeuMs = 0;

    FadeOverlay* overlay = nullptr;
    FadeOverlay* overlayFadeOut = nullptr;
    GestionnaireAudio* gestionnaireAudio = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QPropertyAnimation* fadeOutAnim = nullptr;
    QPropertyAnimation* fadeOutMusique = nullptr;

    QSharedPointer<QPixmap> arrierePlan;
    QPixmap arrierePlanCache;
    
    Reticule* reticule;

    CompteurBalles* compteurBalles = nullptr;
    Vies* vies = nullptr;
    CompteurPoints* compteurPoints = nullptr;

    SDL_Gamepad* gamepad = nullptr;

    bool enPause = false;
    bool transitionVersMenu = false;
    MenuPauseOverlay* menuPause = nullptr;
    QTimer* timerManette = nullptr;

    Jeu* jeu = nullptr;

    int maxBalles;
	Armes* armes;

	bool gachettePrecedente = false;
    bool carrePrecedent     = false;   // debounce bouton carré PS → reload
    bool startPrecedent     = false;   // debounce bouton Start PS → pause
    bool powerUp = false;
    int largeurMaxBalles = 275;
    int largeurMinBalles = 120;

    Touches* touches = nullptr;

    int power_up = 0;
};

#endif