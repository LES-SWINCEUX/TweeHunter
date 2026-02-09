#ifndef PANNEAU_PRINCIPAL_H
#define PANNEAU_PRINCIPAL_H

#include "panneau_menu.h"
#include "bouton.h"

class PanneauPrincipal : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauPrincipal(QWidget* parent = nullptr);
    ~PanneauPrincipal();

protected:
    void creer() override;
    void positionner() override;

private:
    Bouton* boutonJouer = nullptr;
    Bouton* boutonScores = nullptr;
    Bouton* boutonOptions = nullptr;
    Bouton* boutonQuitter = nullptr;

    int espacementBoutons = 25;
    float echelleBoutons = 0.7f;

    void parametrerBoutons(Bouton* bouton, int &hauteur, int &nombre);
    void positionnementBoutons(Bouton* bouton, int& y);
};

#endif