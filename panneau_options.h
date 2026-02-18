#ifndef PANNEAU_OPTIONS_H
#define PANNEAU_OPTIONS_H

#include "panneau_menu.h"
#include "bouton.h"
#include "volume_bouton.h"
#include "gestionnaire_audio.h"

class PanneauOptions : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauOptions(GestionnaireAudio* gestionnaireAudio, QWidget* parent = nullptr);
    ~PanneauOptions();

protected:
    void creer() override;
    void positionner() override;

private:
    VolumeBouton* volumeSFX = nullptr;
    Bouton* retourBouton = nullptr;
    VolumeBouton* volumeMusique = nullptr;

    GestionnaireAudio* gestionnaireAudio = nullptr;

    int espacementBoutons = std::max(10, int(height() * 0.04f));
    float echelleBoutons = 0.7f;
};

#endif