#include "panneau_options.h"

PanneauOptions::PanneauOptions(QAudioOutput* mixeurVolume, float volumeMax, QWidget* parent) : PanneauMenu(parent)
{
    this->volumeMax = volumeMax;
    this->mixeurVolume = mixeurVolume;
    initialiserPanneau();
}

PanneauOptions::~PanneauOptions() {
    std::cout << "PanneauOptions détruit" << std::endl;
}

void PanneauOptions::creer()
{
    retourBouton = new Bouton("/images/menu/retour_spritesheet.png", 3, this);
    retourBouton->setEchelle(echelleBoutons);
    volume = new VolumeBouton("/images/menu/musique_spritesheet.png", this->mixeurVolume, this->volumeMax, this);
    volume->setEchelle(echelleBoutons);

    connect(retourBouton, &Bouton::clicked, this, &PanneauMenu::demanderRetourOptions);
}

static float clampf(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

void PanneauOptions::positionner()
{
    std::vector<Bouton*> boutons = { volume, retourBouton };
    boutons.erase(std::remove(boutons.begin(), boutons.end(), nullptr), boutons.end());
    if (boutons.empty() || !volume) return;

    const QSize base = boutons.front()->tailleImage();
    if (!base.isValid()) return;

    espacementBoutons = std::max(10, int(height() * 0.04f));

    const float boutonHauteur = height() * 0.10f;
    float nouvelleEchelle = boutonHauteur / float(base.height());
    nouvelleEchelle = std::clamp(nouvelleEchelle, 0.25f, 1.2f);

    for (Bouton* bouton : boutons)
        bouton->setEchelle(nouvelleEchelle);

    int hauteurTotale = 0;
    for (Bouton* bouton : boutons) hauteurTotale += bouton->height();
    hauteurTotale += volume->height();
    hauteurTotale += espacementBoutons * (int(boutons.size()));

    int y = (height() - hauteurTotale) / 2;

    for (auto* b : boutons) {
        int x = (width() - b->width()) / 2;
        b->move(x, y);
        y += b->height() + espacementBoutons;
    }
}
