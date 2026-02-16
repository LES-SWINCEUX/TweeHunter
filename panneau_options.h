#ifndef PANNEAU_OPTIONS_H
#define PANNEAU_OPTIONS_H

#include "panneau_menu.h"
#include "bouton.h"
#include "volume_bouton.h"

class PanneauOptions : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauOptions(QAudioOutput* mixeurVolume, float volumeMax, QWidget* parent = nullptr);
    ~PanneauOptions();

protected:
    void creer() override;
    void positionner() override;

private:
    QAudioOutput* mixeurVolume = nullptr;
    Bouton* retourBouton = nullptr;
    VolumeBouton* volume = nullptr;

    int espacementBoutons = std::max(10, int(height() * 0.04f));
    float echelleBoutons = 0.7f;
    float volumeMax = 0.0f;
};

#endif