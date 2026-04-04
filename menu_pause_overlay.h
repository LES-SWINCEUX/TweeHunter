#ifndef MENU_PAUSE_OVERLAY_H
#define MENU_PAUSE_OVERLAY_H

#include <QDir>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "panneau_menu.h"
#include "panneau_options.h"
#include "panneau_scores.h"
#include "sprite_manager.h"
#include <SDL3/SDL.h>
#include "Touches.h"

class MenuPauseOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit MenuPauseOverlay(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr, Touches* touches = nullptr);
    ~MenuPauseOverlay() override = default;

signals:
    void reprendreDemande();
    void retourMenuDemande();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    GestionnaireAudio* gestionnaireAudio = nullptr;
    PanneauMenu* panneau = nullptr;
    QSharedPointer<QPixmap> titreSprite = nullptr;

    QTimer timerAnimationTitre;
    QTimer timerManette;
    QElapsedTimer timerPauseAnimation;

    QRect zonePourPanneau(PanneauMenu* p) const;
    QRect zonePanneauxBas() const;

    void configuerAnimationTitre();

    void afficherPanneauPrincipal();
    void afficherOptions();
    void afficherPanneauScores();

    void afficherTitre(QPainter& painter);

    void initialiserManette();
    void tickManette();

    const int nombreImageTitre = 12;
    const int tempsAnimation = 1000;
    const int tempsAttenteAnimation = 9000;

    const float ratioPanneaux = 0.25f;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;
    bool cacherTitre = false;

    bool dpadHautPrecedent = false;
    bool dpadBasPrecedent = false;
    bool boutonOkPrecedent = false;

    SDL_Gamepad* gamepad = nullptr;

    Touches* touchesPerso = nullptr;
    bool customHautPrecedent = false;
    bool customBasPrecedent = false;
    bool customOkPrecedent = false;
};

#endif
