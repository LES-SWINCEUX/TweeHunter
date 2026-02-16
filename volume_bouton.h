#ifndef VOLUME_BOUTON_H
#define VOLUME_BOUTON_H

#include "bouton.h"

#include <QAudioOutput>
#include <QPainter>
#include <QKeyEvent>
#include <QDir>

class VolumeBouton : public Bouton {
    Q_OBJECT
public:

    VolumeBouton(const QString& cheminBouton, QAudioOutput* mixeurVolume, float volumeMax, QWidget* parent = nullptr);

    void setVolume(float v);
    float volume() const { return volume_; }

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QAudioOutput* mixeurVolume = nullptr;
    float volumeMax = 0.0f;
    float volume_ = 0.5714f;
};

#endif