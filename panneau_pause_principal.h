#ifndef PANNEAU_PAUSE_PRINCIPAL_H
#define PANNEAU_PAUSE_PRINCIPAL_H

#include <algorithm>
#include <iostream>

#include "panneau_menu.h"
#include "bouton.h"

class PanneauPausePrincipal : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauPausePrincipal(bool manetteConnectee = false, QWidget* parent = nullptr);
    ~PanneauPausePrincipal() override;

    QList<Bouton*> boutonsNavigables() const override;

protected:
    void creer() override;
    void positionner() override;

private:
    Bouton* boutonJouer = nullptr;
    Bouton* boutonScores = nullptr;
    Bouton* boutonOptions = nullptr;
    Bouton* boutonQuitter = nullptr;

    int espacementBoutons = 0;
    float echelleBoutons = 0.7f;
};

#endif
