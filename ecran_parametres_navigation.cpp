// ecran_parametres_navigation.cpp
// Responsabilité : navigation au clavier et à la manette, gestion du focus.

#include "ecran_parametres.h"

QRect EcranParametres::rectNavigable(QWidget* widget) const
{
    if (!widget) {
        return QRect();
    }

    if (widget == champNom) {
        return rectFondNom;
    }

    return widget->geometry();
}

QPoint EcranParametres::centreNavigable(QWidget* widget) const
{
    return rectNavigable(widget).center();
}

int EcranParametres::trouverProchainIndexDansDirection(int dx, int dy) const
{
    if (widgetsNavigables.empty()) {
        return -1;
    }

    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        for (int i = 0; i < int(widgetsNavigables.size()); ++i) {
            QWidget* w = widgetsNavigables[i];
            if (w && w->isVisible() && w->isEnabled()) {
                return i;
            }
        }
        return -1;
    }

    QWidget* courant = widgetsNavigables[indexFocus];
    if (!courant) {
        return -1;
    }

    const QPoint centreCourant = centreNavigable(courant);

    int meilleurIndex = -1;
    double meilleurScore = std::numeric_limits<double>::max();

    for (int i = 0; i < int(widgetsNavigables.size()); ++i) {
        if (i == indexFocus) {
            continue;
        }

        QWidget* candidat = widgetsNavigables[i];
        if (!candidat || !candidat->isVisible() || !candidat->isEnabled()) {
            continue;
        }

        const QPoint centreCandidat = centreNavigable(candidat);
        const int vx = centreCandidat.x() - centreCourant.x();
        const int vy = centreCandidat.y() - centreCourant.y();

        int primaire = 0;
        int secondaire = 0;

        if (dx != 0) {
            primaire = vx * dx;
            secondaire = std::abs(vy);
        }
        else {
            primaire = vy * dy;
            secondaire = std::abs(vx);
        }

        if (primaire <= 0) {
            continue;
        }

        double score = double(primaire) + double(secondaire) * 2.75;

        if (secondaire < 24) {
            score *= 0.75;
        }

        if (score < meilleurScore) {
            meilleurScore = score;
            meilleurIndex = i;
        }
    }

    return meilleurIndex;
}

void EcranParametres::deplacerFocusDirection(int dx, int dy)
{
    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        const int premier = trouverProchainIndexDansDirection(dx, dy);
        if (premier >= 0) {
            appliquerFocus(premier);
        }
        return;
    }

    QWidget* courant = widgetsNavigables[indexFocus];
    if (!courant) {
        return;
    }

    auto focusWidget = [this](QWidget* cible) -> bool
    {
        if (!cible || !cible->isVisible() || !cible->isEnabled()) {
            return false;
        }

        const auto it = std::find(widgetsNavigables.begin(), widgetsNavigables.end(), cible);
        if (it == widgetsNavigables.end()) {
            return false;
        }

        appliquerFocus(int(std::distance(widgetsNavigables.begin(), it)));
        return true;
    };

    auto focusNom = [this, &focusWidget]() -> bool
    {
        return focusWidget(champNom);
    };

    auto manettesVisibles = [this]() -> std::vector<QWidget*>
    {
        std::vector<QWidget*> resultat;

        if (boutonManetteStandard && boutonManetteStandard->isVisible() && boutonManetteStandard->isEnabled()) {
            resultat.push_back(boutonManetteStandard);
        }
        if (boutonManetteCustom && boutonManetteCustom->isVisible() && boutonManetteCustom->isEnabled()) {
            resultat.push_back(boutonManetteCustom);
        }
        if (boutonClavierSouris && boutonClavierSouris->isVisible() && boutonClavierSouris->isEnabled()) {
            resultat.push_back(boutonClavierSouris);
        }

        return resultat;
    };

    auto premiereManetteVisible = [&]() -> QWidget*
    {
        auto liste = manettesVisibles();
        return liste.empty() ? nullptr : liste.front();
    };

    auto deplacerDansRangee = [&](const std::vector<QWidget*>& rangee, int direction) -> bool
    {
        if (direction == 0) {
            return false;
        }

        auto it = std::find(rangee.begin(), rangee.end(), courant);
        if (it == rangee.end()) {
            return false;
        }

        const int index = int(std::distance(rangee.begin(), it));
        const int suivant = index + direction;

        if (suivant < 0 || suivant >= int(rangee.size())) {
            return true;
        }

        return focusWidget(rangee[suivant]);
    };

    const std::vector<QWidget*> rangeeMilieu = {
        boutonMode18,
        boutonModeNormal,
        boutonDifficulteNormal,
        boutonDifficulteRng,
        boutonDifficulteChaos
    };

    const std::vector<QWidget*> rangeeBoutonsBas = {
        boutonRetour,
        boutonCommencer
    };

    const auto rangeeManette = manettesVisibles();

    const bool focusSurBlocMilieu = courant == boutonMode18 || courant == boutonModeNormal || courant == boutonDifficulteNormal || courant == boutonDifficulteRng || courant == boutonDifficulteChaos;

    const bool focusSurManette = courant == boutonManetteStandard || courant == boutonManetteCustom || courant == boutonClavierSouris;

    const bool focusSurBoutonsBas = courant == boutonRetour || courant == boutonCommencer;

    if (dx != 0) {
        if (courant == champNom) {
            return;
        }

        if (deplacerDansRangee(rangeeMilieu, dx)) {
            return;
        }

        if (deplacerDansRangee(rangeeManette, dx)) {
            return;
        }

        if (deplacerDansRangee(rangeeBoutonsBas, dx)) {
            return;
        }
    }

    if (dy > 0 && focusSurBlocMilieu) {
        if (focusWidget(premiereManetteVisible())) {
            return;
        }
    }

    if (dy > 0 && focusSurManette) {
        if (focusNom()) {
            return;
        }
    }

    if (dy < 0 && focusSurBoutonsBas) {
        if (focusNom()) {
            return;
        }
    }

    if (dy < 0 && courant == champNom) {
        if (focusWidget(premiereManetteVisible())) {
            return;
        }
    }

    if (dy > 0 && courant == champNom) {
        if (focusWidget(boutonCommencer)) {
            return;
        }
    }

    const int prochain = trouverProchainIndexDansDirection(dx, dy);
    if (prochain >= 0) {
        appliquerFocus(prochain);
    }
}

void EcranParametres::appliquerFocus(int nouvelIndex)
{
    if (nouvelIndex < 0 || nouvelIndex >= int(widgetsNavigables.size())) {
        return;
    }

    QWidget* cible = widgetsNavigables[nouvelIndex];
    if (!cible || !cible->isVisible() || !cible->isEnabled()) {
        return;
    }

    if (indexFocus >= 0 && indexFocus < int(widgetsNavigables.size())) {
        if (auto* bouton = qobject_cast<Bouton*>(widgetsNavigables[indexFocus])) {
            bouton->setSelectionneManette(false);
        }
        else if (widgetsNavigables[indexFocus] && widgetsNavigables[indexFocus]->hasFocus()) {
            widgetsNavigables[indexFocus]->clearFocus();
            widgetsNavigables[indexFocus]->update();
        }
    }

    indexFocus = nouvelIndex;

    QWidget* widget = widgetsNavigables[indexFocus];
    if (auto* bouton = qobject_cast<Bouton*>(widget)) {
        bouton->setSelectionneManette(true);
    }
    else if (widget) {
        widget->setFocus(Qt::OtherFocusReason);
        widget->update();
    }

    update();
}

void EcranParametres::confirmerFocus()
{
    if (indexFocus < 0 || indexFocus >= int(widgetsNavigables.size())) {
        return;
    }

    QWidget* widget = widgetsNavigables[indexFocus];

    if (auto* boutonSprite = qobject_cast<Bouton*>(widget)) {
        boutonSprite->simulerClic();
        return;
    }

    if (auto* boutonQt = qobject_cast<QPushButton*>(widget)) {
        boutonQt->click();
        return;
    }

    if (widget == champNom) {
        champNom->setFocus(Qt::OtherFocusReason);
    }
}

void EcranParametres::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    if (transitionEnCours) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
        deplacerFocusDirection(-1, 0);
        event->accept();
        return;

    case Qt::Key_Right:
        deplacerFocusDirection(1, 0);
        event->accept();
        return;

    case Qt::Key_Up:
        deplacerFocusDirection(0, -1);
        event->accept();
        return;

    case Qt::Key_Down:
        deplacerFocusDirection(0, 1);
        event->accept();
        return;

    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        confirmerFocus();
        event->accept();
        return;

    case Qt::Key_Escape:
        lancerRetourMenu();
        event->accept();
        return;

    default:
        break;
    }

    QWidget::keyPressEvent(event);
}
