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

class EcranJeu : public QWidget
{
    Q_OBJECT

public:
    EcranJeu(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr);
    ~EcranJeu();
    void tire();

signals:
    void finPartie(int score);

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

    QTimer timer;
    QElapsedTimer elapsed;

    FadeOverlay* overlay = nullptr;
    GestionnaireAudio* gestionnaireAudio = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;

    QSharedPointer<QPixmap> arrierePlan;
    QPixmap arrierePlanCache;
    
    Reticule* reticule;

    CompteurBalles* compteurBalles = nullptr;
    Vies* vies = nullptr;
    CompteurPoints* compteurPoints = nullptr;

    SDL_Gamepad* gamepad = nullptr;

    Jeu* jeu = nullptr;

    const int maxBalles = 9;

	bool gachettePrecedente = false;
    int largeurMaxBalles = 275;
    int largeurMinBalles = 120;
};

#endif