#ifndef ECRAN_REGLES_H
#define ECRAN_REGLES_H

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPropertyAnimation>
#include <QVector>
#include <QColor>
#include <QRect>
#include <QString>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QFont>
#include <functional>
#include <QPainter>
#include <QPen>
#include <QDir>
#include <QFontMetrics>
#include <QEasingCurve>
#include <cmath>

#include "bouton.h"
#include "configuration_partie.h"
#include "fade_overlay.h"
#include "gestionnaire_audio.h"
#include "Touches.h"
#include "sprite_manager.h"

class EcranRegles : public QWidget
{
    Q_OBJECT

public:
    EcranRegles(GestionnaireAudio* gestionnaireAudio, Touches* touches, QWidget* parent = nullptr);
    ~EcranRegles() = default;

    void setConfiguration(const ConfigurationPartie& config);

signals:
    void demarrerPartieDemande(const ConfigurationPartie& configuration);
    void retourParametresDemande();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    struct CarteRegle
    {
        QString titre;
        QString description;
        QColor couleurAccent;

        QVector<QVector<QPixmap>> animations;

        int indexAnimation = 0;
        int indexFrame = 0;
        int tempsAccumuleFrameMs = 0;
        int dureeFrameMs = 110;

        bool changerAnimationApresCycle = false;

        float phaseOscillation = 0.0f;
        int amplitudeOscillation = 8;
    };

    struct MiseEnPage
    {
        QRect panneau;
        QRect titre;
        QRect separateur;
        QVector<QRect> cartes;
    };

    void creerInterface();
    void connecterSignaux();
    void initialiserCartes();

    void ajusterTailleBoutons();
    void placerElements();

    void mettreAJourFondCache();
    void mettreAJourMiseEnPage();

    void demarrerAnimation();
    void arreterAnimation();
    void mettreAJourAnimation();
    void avancerAnimationCarte(CarteRegle& carte, int deltaMs);

    void dessinerArrierePlan(QPainter& painter);
    void dessinerPanneau(QPainter& painter);
    void dessinerTitre(QPainter& painter);
    void dessinerSeparateur(QPainter& painter);
    void dessinerCartes(QPainter& painter);
    void dessinerCarte(QPainter& painter, const QRect& rect, const CarteRegle& carte);

    QVector<QPixmap> extraireFramesSpritesheet(const QString& cheminSprite,
        int colonnes,
        int lignes);

    QVector<QString> obtenirCheminsPoisonSelonMode(ModeJeu mode) const;
    QVector<QString> obtenirCheminsGatorSelonMode(ModeJeu mode) const;
    QVector<QString> obtenirCheminsDebuffSelonMode(ModeJeu mode) const;
    QVector<QString> obtenirCheminsWaterSelonMode(ModeJeu mode) const;

    const QPixmap* obtenirFrameActuelle(const CarteRegle& carte) const;
    int calculerOffsetVertical(const CarteRegle& carte) const;
    int bornerIndex(int valeur, int minimum, int maximum) const;
    int calculerHauteurCarte(const CarteRegle& carte, int largeurCarte) const;

    QFont creerPoliceCompteur(int pixelSize) const;
    QFont creerPoliceTexte(int pixelSize, bool gras = false) const;

    void lancerTransition(const std::function<void()>& actionFinale, bool fadeMusique);
    void lancerDemarrage();
    void lancerRetour();

    const int kColonnesSpritesheet = 4;
    const int kLignesSpritesheet = 3;

    const QVector<QString> kSpritesPoisonPlus18 = {
        "/images/sprites/poisonbrun.png",
        "/images/sprites/poisonvert.png"
    };

    const QVector<QString> kSpritesPoisonMoins18 = {
        "/images/sprites/poisonbrun.png",
        "/images/sprites/poisonvert.png"
    };

    const QVector<QString> kSpritesGatorPlus18 = {
        "/images/sprites/gator.png"
    };

    const QVector<QString> kSpritesGatorMoins18 = {
        "/images/sprites/gator.png"
    };

    const QVector<QString> kSpritesDebuffPlus18 = {
        "/images/sprites/jack_daniels.png",
        "/images/sprites/grey_goose.png",
        "/images/sprites/jimador.png",
        "/images/sprites/guiness.png",
        "/images/sprites/creme_menthe.png",
        "/images/sprites/baileys.png"
    };

    const QVector<QString> kSpritesDebuffMoins18 = {
        "/images/sprites/coca_cola.png",
        "/images/sprites/fuze.png"
    };

    const QVector<QString> kSpritesWaterPlus18 = {
        "/images/sprites/water.png"
    };

    const QVector<QString> kSpritesWaterMoins18 = {
        "/images/sprites/water.png"
    };

    GestionnaireAudio* gestionnaireAudio = nullptr;
    Touches* touches = nullptr;
    ConfigurationPartie configuration;

    QSharedPointer<QPixmap> arrierePlan;
    QPixmap arrierePlanCache;

    QVector<CarteRegle> cartesRegles;
    MiseEnPage miseEnPage;

    Bouton* boutonCommencer = nullptr;
    Bouton* boutonRetour = nullptr;

    FadeOverlay* overlay = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QPropertyAnimation* fadeOutAnim = nullptr;
    QPropertyAnimation* fadeOutMusique = nullptr;

    QTimer* timerAnimation = nullptr;
    int tempsAnimationMs = 0;

    bool transitionEnCours = false;
};

#endif