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

#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "jeu.h"
#include "Reticule.h"

#include <SDL3/SDL.h>
#include <QTimer>
#include <QDebug>
#include <cmath>

class EcranJeu : public QWidget
{
    Q_OBJECT

public:
    EcranJeu(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr);
    ~EcranJeu();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void mouseMoveEvent(QMouseEvent* event) override;


private:
    void tick();

    QTimer timer;
    QElapsedTimer elapsed;

    FadeOverlay* overlay = nullptr;
    GestionnaireAudio* gestionnaireAudio = nullptr;

    QPropertyAnimation* estompeMusique = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;

    QPixmap background;
    
    Reticule* reticule;


    SDL_Gamepad* gamepad = nullptr;

    Jeu* jeu = nullptr;
};

#endif