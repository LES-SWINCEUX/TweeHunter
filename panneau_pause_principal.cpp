#include "panneau_pause_principal.h"

#include <algorithm>
#include <iostream>

PanneauPausePrincipal::PanneauPausePrincipal(QWidget* parent)
    : PanneauMenu(parent)
{
    initialiserPanneau();
    naviguerBas();
}

PanneauPausePrincipal::~PanneauPausePrincipal() {}

void PanneauPausePrincipal::creer()
{
    boutonJouer = new Bouton("/images/menu/jouer_spritesheet_horizontal.png", 3, this);
    boutonJouer->setEchelle(echelleBoutons);

    boutonScores = new Bouton("/images/menu/scores_spritesheet_horizontal.png", 3, this);
    boutonScores->setEchelle(echelleBoutons);

    boutonOptions = new Bouton("/images/menu/options_spritesheet_horizontal.png", 3, this);
    boutonOptions->setEchelle(echelleBoutons);

    boutonQuitter = new Bouton("/images/menu/quitter_spritesheet_horizontal.png", 3, this);
    boutonQuitter->setEchelle(echelleBoutons);

    connect(boutonJouer,   &Bouton::clicked, this, &PanneauMenu::demanderJouer);
    connect(boutonScores,  &Bouton::clicked, this, &PanneauMenu::demanderScores);
    connect(boutonOptions, &Bouton::clicked, this, &PanneauMenu::demanderOptions);
    connect(boutonQuitter, &Bouton::clicked, this, &PanneauMenu::demanderQuitter);
}

QList<Bouton*> PanneauPausePrincipal::boutonsNavigables() const
{
    return { boutonJouer, boutonScores, boutonOptions, boutonQuitter };
}

void PanneauPausePrincipal::positionner()
{
    std::vector<Bouton*> boutons = { boutonJouer, boutonScores, boutonOptions, boutonQuitter };
    boutons.erase(std::remove(boutons.begin(), boutons.end(), nullptr), boutons.end());
    if (boutons.empty()) {
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

    for (Bouton* b : boutons) {
        b->setEchelle(nouvelleEchelle);
    }

    int hauteurTotale = 0;
    for (Bouton* b : boutons) {
        hauteurTotale += b->height();
    }
    hauteurTotale += espacementBoutons * (int(boutons.size()) - 1);

    int y = (height() - hauteurTotale) / 2;

    for (Bouton* b : boutons) {
        int x = (width() - b->width()) / 2;
        b->move(x, y);
        y += b->height() + espacementBoutons;
    }
}
