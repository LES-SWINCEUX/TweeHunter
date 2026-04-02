#include "panneau_menu.h"

void PanneauMenu::appliquerFocus(int nouvelIndex)
{
    QList<Bouton*> boutons = boutonsNavigables();

    // Deselectioner l'ancien
    if (indexFocus >= 0 && indexFocus < boutons.size()) {
        boutons[indexFocus]->setSelectionneManette(false);
    }

    indexFocus = nouvelIndex;

    // Selectionner le nouveau
    if (indexFocus >= 0 && indexFocus < boutons.size()) {
        boutons[indexFocus]->setSelectionneManette(true);
    }
}

void PanneauMenu::naviguerHaut()
{
    QList<Bouton*> boutons = boutonsNavigables();
    if (boutons.isEmpty()) {
        return;
    }

    int nouvel = (indexFocus <= 0) ? boutons.size() - 1 : indexFocus - 1;
    appliquerFocus(nouvel);
}

void PanneauMenu::naviguerBas()
{
    QList<Bouton*> boutons = boutonsNavigables();
    if (boutons.isEmpty()) {
        return;
    }

    int nouvel = (indexFocus < 0 || indexFocus >= boutons.size() - 1) ? 0 : indexFocus + 1;
    appliquerFocus(nouvel);
}

void PanneauMenu::confirmer()
{
    QList<Bouton*> boutons = boutonsNavigables();
    if (indexFocus >= 0 && indexFocus < boutons.size()) {
        boutons[indexFocus]->simulerClic();
    }
}

void PanneauMenu::reinitialiserFocus()
{
    appliquerFocus(-1);
}

void PanneauMenu::focusSurIndex(int index)
{
    appliquerFocus(index);
}
