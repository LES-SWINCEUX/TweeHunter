#include "panneau_principal.h"

PanneauPrincipal::PanneauPrincipal(QWidget* parent) : PanneauMenu(parent)
{
    initialiserPanneau();
}

PanneauPrincipal::~PanneauPrincipal() {
    delete boutonJouer;
    delete boutonScores;
    delete boutonOptions;
    delete boutonQuitter;
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

void PanneauPrincipal::positionner()
{
    int hauteurTotale = 0;
    int nombreBoutons = 0;

    parametrerBoutons(boutonJouer, hauteurTotale, nombreBoutons);
    parametrerBoutons(boutonScores, hauteurTotale, nombreBoutons);
    parametrerBoutons(boutonOptions, hauteurTotale, nombreBoutons);
    parametrerBoutons(boutonQuitter, hauteurTotale, nombreBoutons);

    if (nombreBoutons > 1) {
        hauteurTotale += espacementBoutons * (nombreBoutons - 1);
    }
    if (nombreBoutons == 0) {
        return;
    }

    int y = (height() - hauteurTotale) / 2;

    positionnementBoutons(boutonJouer, y);
    positionnementBoutons(boutonScores, y);
    positionnementBoutons(boutonOptions, y);
    positionnementBoutons(boutonQuitter, y);
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
