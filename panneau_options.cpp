#include "panneau_options.h"

PanneauOptions::PanneauOptions(GestionnaireAudio* gestionnaireAudio, QWidget* parent) : PanneauMenu(parent)
{
    this->gestionnaireAudio = gestionnaireAudio;
    initialiserPanneau();
}

PanneauOptions::~PanneauOptions() {
    std::cout << "PanneauOptions détruit" << std::endl;
}

void PanneauOptions::creer()
{
    volumeSFX = new VolumeBouton("/images/menu/sfx_spritesheet.png", this->gestionnaireAudio, SFX, this);
    volumeSFX->setEchelle(echelleBoutons);

    retourBouton = new Bouton("/images/menu/retour_spritesheet.png", 3, this);
    retourBouton->setEchelle(echelleBoutons);

    volumeMusique = new VolumeBouton("/images/menu/musique_spritesheet.png", this->gestionnaireAudio, MUSIQUE, this);
    volumeMusique->setEchelle(echelleBoutons);

    connect(retourBouton, &Bouton::clicked, this, &PanneauMenu::demanderRetourOptions);
}

static float clampf(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

void PanneauOptions::positionner()
{
    std::vector<Bouton*> boutons = { volumeMusique, volumeSFX, retourBouton };
    boutons.erase(std::remove(boutons.begin(), boutons.end(), nullptr), boutons.end());
    if (boutons.empty() || !volumeMusique) {
        return;
    }

    const QSize base = boutons.front()->tailleImage();
    if (!base.isValid()) {
        return;
    }

    espacementBoutons = std::max(10, int(height() * 0.04f));

    const float boutonHauteur = height() * 0.10f;
    float nouvelleEchelle = boutonHauteur / float(base.height());
    nouvelleEchelle = std::clamp(nouvelleEchelle, 0.25f, 1.2f);

    for (Bouton* bouton : boutons) {
        bouton->setEchelle(nouvelleEchelle);
    }

    int hauteurTotale = 0;
    for (Bouton* bouton : boutons) {
        hauteurTotale += bouton->height();
    }

    hauteurTotale += espacementBoutons * (int(boutons.size()));

    int y = (height() - hauteurTotale) / 2;

    for (Bouton* bouton : boutons) {
        int x = (width() - bouton->width()) / 2;
        bouton->move(x, y);
        y += bouton->height() + espacementBoutons;
    }
}
