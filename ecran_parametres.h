#ifndef ECRAN_PARAMETRES_H
#define ECRAN_PARAMETRES_H

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QDir>
#include <QResizeEvent>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QTimer>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QFont>
#include <vector>
#include <QApplication>
#include <QSignalBlocker>

#include <algorithm>
#include <cmath>
#include <limits>

#include <SDL3/SDL.h>

#include "bouton.h"
#include "configuration_partie.h"
#include "fade_overlay.h"
#include "sprite_manager.h"
#include "Touches.h"
#include "gestionnaire_audio.h"
#include "bouton_options.h"
#include "gestionnaire_configuration.h"

class EcranParametres : public QWidget
{
    Q_OBJECT

public:
    EcranParametres(GestionnaireAudio* gestionnaireAudio, Touches* touches, QWidget* parent = nullptr);
    ~EcranParametres() = default;
    void chargerConfiguration(const ConfigurationPartie& config);

signals:
    void demarrerPartieDemande(const ConfigurationPartie& configuration);
    void retourMenuDemande(ConfigurationPartie configuration);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void creerInterface();
    void connecterSignaux();
    void placerElements();
    void appliquerEtatVisuel();
    void centrerContenuVerticalement();
    void appliquerDisponibiliteManettes();

    TypeManette premiereManetteDisponible() const;

    int placerGrille(const std::vector<Bouton*>& boutons, int colonnes, int x, int y, int largeurDisponible, int hauteurBoutonCible, int espacementX, int espacementY);

    QRect calculerRectTitre(const QSharedPointer<QPixmap>& pixmap, int centreX, int y, int hauteurCible, int largeurMax) const;

    void dessinerPixmap(QPainter& painter, const QSharedPointer<QPixmap>& pixmap, const QRect& destination) const;

    void lancerDemarrage();
    void lancerRetourMenu();
    ConfigurationPartie configurationFinale() const;

    void appliquerFocus(int index);
    void confirmerFocus();
    void tickManette();

    QRect rectNavigable(QWidget* widget) const;
    QPoint centreNavigable(QWidget* widget) const;
    int trouverProchainIndexDansDirection(int dx, int dy) const;
    void deplacerFocusDirection(int dx, int dy);
    void gererNavigationJoystick(float axeX, float axeY);

    bool verrouNavigationJoystick = false;
    bool verrouNavigationJoystickCustom = false;

    GestionnaireAudio* gestionnaireAudio = nullptr;
    Touches* touches = nullptr;

    ConfigurationPartie configuration;

    QSharedPointer<QPixmap> arrierePlan;
    QSharedPointer<QPixmap> titrePrincipalImg;
    QSharedPointer<QPixmap> titreChoixArmeImg;
    QSharedPointer<QPixmap> titreModeJeuImg;
    QSharedPointer<QPixmap> titrePowerUpsImg;
    QSharedPointer<QPixmap> titreDifficulteImg;
    QSharedPointer<QPixmap> titreChoixManetteImg;
    QSharedPointer<QPixmap> titreNomImg;
    QSharedPointer<QPixmap> fondNomImg;

    QPixmap arrierePlanCache;

    QRect rectTitrePrincipal;
    QRect rectTitreArmes;
    QRect rectTitreModeJeu;
    QRect rectTitrePowerUps;
    QRect rectTitreDifficulte;
    QRect rectTitreChoixManette;
    QRect rectTitreNom;
    QRect rectFondNom;

    Bouton* boutonCarabine = nullptr;
    Bouton* boutonShotgun = nullptr;
    Bouton* boutonBombardier = nullptr;
    Bouton* boutonGrpc = nullptr;
    Bouton* boutonTarte = nullptr;
    Bouton* boutonSwince = nullptr;

    BoutonOptions* boutonMode18 = nullptr;
    BoutonOptions* boutonModeNormal = nullptr;

    BoutonOptions* boutonDifficulteNormal = nullptr;
    BoutonOptions* boutonDifficulteRng = nullptr;
    BoutonOptions* boutonDifficulteChaos = nullptr;

    Bouton* boutonGrenade = nullptr;
    Bouton* boutonZap = nullptr;
    Bouton* boutonMitraillette = nullptr;
    Bouton* boutonTacticalNuke = nullptr;

    Bouton* boutonManetteStandard = nullptr;
    Bouton* boutonManetteCustom = nullptr;
    Bouton* boutonClavierSouris = nullptr;

    Bouton* boutonCommencer = nullptr;
    Bouton* boutonRetour = nullptr;

    QLineEdit* champNom = nullptr;

    std::vector<QWidget*> widgetsNavigables;
    int indexFocus = -1;

    FadeOverlay* overlay = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QPropertyAnimation* fadeOutAnim = nullptr;
    QPropertyAnimation* fadeOutMusique = nullptr;

    QTimer timerManette;
    bool hautPrecedent = false;
    bool basPrecedent = false;
    bool gauchePrecedent = false;
    bool droitePrecedent = false;
    bool okPrecedent = false;

    bool customHautPrecedent = false;
    bool customBasPrecedent = false;
    bool customOkPrecedent = false;

    bool transitionEnCours = false;
    QFont policeTitre;
};

#endif
