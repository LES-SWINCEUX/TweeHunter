#ifndef VOLUME_BOUTON_H
#define VOLUME_BOUTON_H

#include "bouton.h"
#include "gestionnaire_audio.h"

#include <QPainterPath>
#include <QFontMetrics>
#include <QPainter>
#include <QKeyEvent>
#include <QDir>

class VolumeBouton : public Bouton {
    Q_OBJECT
public:
    VolumeBouton(const QString& cheminBouton, GestionnaireAudio* gestionnaireAudio, AudioMode mode, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    const int volumeBars = 7;

    GestionnaireAudio* gestionnaireAudio = nullptr;
    AudioMode mode;

    float parseVolume(float volume);
    void setVolume(float v);
    void onButtonClick();
};

#endif