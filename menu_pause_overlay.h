#ifndef MENU_PAUSE_OVERLAY_H
#define MENU_PAUSE_OVERLAY_H

#include <QDir>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>

#include "panneau_pause_principal.h"
#include "panneau_menu.h"
#include "panneau_options.h"
#include "panneau_scores.h"
#include "sprite_manager.h"
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
    QRect zonePourPanneau(PanneauMenu* p) const;
    QRect zonePanneauxBas() const;

    void configuerAnimationTitre();

    void afficherPanneauPrincipal();
    void afficherOptions();
    void afficherPanneauScores();

    void afficherTitre(QPainter& painter);

    void initialiserManette();
    bool manetteConnectee() const;

    GestionnaireAudio* gestionnaireAudio = nullptr;
    PanneauMenu* panneau = nullptr;
    QSharedPointer<QPixmap> titreSprite = nullptr;

    QTimer timerAnimationTitre;
    QTimer timerManette;
    QElapsedTimer timerPauseAnimation;

    int indexImageTitre = 0;
    int imagesAffichees = 0;

    bool animationActive = true;
    bool fadeEnCours = false;
    bool cacherTitre = false;

    Touches* touches = nullptr;

    const int NOMBRE_IMAGE_TITRE = 12;
    const int TEMPS_ATTENTE_ANIMATION = 9000;
    const int INTERVALE_TITRE_MS = 55; // ~18 fps pour l'animation du titre

    const float RATIO_PANNEAUX = 0.25f;
};

#endif
