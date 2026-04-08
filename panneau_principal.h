#ifndef PANNEAU_PRINCIPAL_H
#define PANNEAU_PRINCIPAL_H

#include "panneau_menu.h"
#include "bouton.h"

class PanneauPrincipal : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauPrincipal(bool manetteConnectee = false, QWidget* parent = nullptr);
    ~PanneauPrincipal();

    QList<Bouton*> boutonsNavigables() const override;

protected:
    void creer() override;
    void positionner() override;

private:
    void parametrerBoutons(Bouton* bouton, int& hauteur, int& nombre);
    void positionnementBoutons(Bouton* bouton, int& y);

    Bouton* boutonJouer = nullptr;
    Bouton* boutonScores = nullptr;
    Bouton* boutonOptions = nullptr;
    Bouton* boutonQuitter = nullptr;

    int espacementBoutons = std::max(10, int(height() * 0.04f));
    float echelleBoutons = 0.7f;
};

#endif