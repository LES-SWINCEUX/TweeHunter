#include "panneau_principal.h"

PanneauPrincipal::PanneauPrincipal(QWidget* parent) : PanneauMenu(parent)
{
    initialiserPanneau();
}

PanneauPrincipal::~PanneauPrincipal() {
}

void PanneauPrincipal::creer()
{
    boutonJouer = new Bouton("/images/menu/jouer_spritesheet_horizontal.png", 3, this);
    boutonJouer->setEchelle(echelleBoutons);

    boutonScores = new Bouton("/images/menu/scores_spritesheet_horizontal.png", 3, this);
    boutonScores->setEchelle(echelleBoutons);

    boutonOptions = new Bouton("/images/menu/options_spritesheet_horizontal.png", 3, this);
    boutonOptions->setEchelle(echelleBoutons);

    boutonQuitter = new Bouton("/images/menu/quitter_spritesheet_horizontal.png", 3, this);
    boutonQuitter->setEchelle(echelleBoutons);

    connect(boutonJouer, &Bouton::clicked, this, &PanneauMenu::demanderJouer);
    connect(boutonScores, &Bouton::clicked, this, &PanneauMenu::demanderScores);
    connect(boutonOptions, &Bouton::clicked, this, &PanneauMenu::demanderOptions);
    connect(boutonQuitter, &Bouton::clicked, this, &PanneauMenu::demanderQuitter);
}

static float clampf(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

void PanneauPrincipal::positionner()
{
    std::vector<Bouton*> boutons = { boutonJouer, boutonScores, boutonOptions, boutonQuitter };
    boutons.erase(std::remove(boutons.begin(), boutons.end(), nullptr), boutons.end());
    if (boutons.empty()) return;

    const QSize base = boutons.front()->tailleImage();
    if (!base.isValid()) return;

    const int nombreBoutons = int(boutons.size());

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
    hauteurTotale += espacementBoutons * (nombreBoutons - 1);

    int y = (height() - hauteurTotale) / 2;

    for (auto* b : boutons) {
        int x = (width() - b->width()) / 2;
        b->move(x, y);
        y += b->height() + espacementBoutons;
    }
}

void PanneauPrincipal::parametrerBoutons(Bouton* bouton, int &hauteur, int &total) {
    if (bouton == nullptr) {
        return;
    }
    hauteur += bouton->height();
    ++total;
}

void PanneauPrincipal::positionnementBoutons(Bouton* bouton, int& y) {
    if (bouton == nullptr) {
        return;
    }

    int x = (width() - bouton->width()) / 2;
    bouton->move(x, y);
    y += bouton->height() + espacementBoutons;
}
