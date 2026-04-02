#ifndef ECRAN_FIN_PARTIE_H
#define ECRAN_FIN_PARTIE_H

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QDir>
#include <QResizeEvent>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QLabel>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <algorithm>

#include "fade_overlay.h"
#include "sprite_manager.h"
#include "bouton.h"
#include "gestionnaire_audio.h"

class EcranFinPartie : public QWidget
{
    Q_OBJECT

public:
    EcranFinPartie(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr);
    ~EcranFinPartie() = default;

    void setScore(int score);
    void setNomParDefaut(const QString& nom);

signals:
    void retourMenuDemande(const QString& nomJoueur, int score);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;

private:
    void placerElements();
    void lancerFadeIn();
    QRect srcRectToScreen(int screenW, int screenH, int srcX, int srcY, int srcW, int srcH);

    QPixmap buildCache(const QSharedPointer<QPixmap>& src);
    QLabel* makeLabel(const QString& txt, const QString& couleur);
    QRect geometrieLabel(float ratioY, float ratioH, float ratioW, int zoneW, int panX, int panY, int panW, int panH);

    GestionnaireAudio* gestionnaireAudio = nullptr;

    QSharedPointer<QPixmap> arrierePlan;
    QPixmap arrierePlanCache;

    QSharedPointer<QPixmap> panneauImg;
    QPixmap panneauCache;

    QSharedPointer<QPixmap> titreImg;
    QPixmap titreCache;

    FadeOverlay* overlay = nullptr;
    QPropertyAnimation* fadeAnim = nullptr;

    int score = 0;
    QString nomParDefaut;
    QRect panneau;

    QLabel* labelVotreScore = nullptr;
    QLabel* labelScore = nullptr;
    QLabel* labelNom = nullptr;
    QLineEdit* champNom = nullptr;
    Bouton* boutonValider = nullptr;

    QFont fontPixel;

    const int SRC_W = 1536;
    const int SRC_H = 1024;
    const int PAN_SRC_X = 339;
    const int PAN_SRC_Y = 225;
    const int PAN_SRC_W = 820;
    const int PAN_SRC_H = 522;

    const float LABEL_SCORE_Y = 0.12f;
    const float LABEL_SCORE_H = 0.16f;
    const float SCORE_VAL_Y = 0.28f;
    const float SCORE_VAL_H = 0.22f;
    const float LABEL_NOM_Y = 0.54f;
    const float LABEL_NOM_H = 0.14f;
    const float CHAMP_Y = 0.68f;
    const float CHAMP_H = 0.14f;
    const float CHAMP_PAD_X = 0.07f;
    const float BTN_Y = 0.86f;
    const float BTN_H_RATIO = 0.17f;
};

#endif
