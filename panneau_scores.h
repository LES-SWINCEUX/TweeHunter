#ifndef PANNEAU_SCORES_H
#define PANNEAU_SCORES_H

#include "panneau_menu.h"
#include "gestionnaire_scores.h"
#include "bouton.h"
#include "texte_menu.h"

#include <QLabel>
#include <QFont>
#include <QFontDatabase>
#include <QDir>
#include <QPainter>
#include <QPixmap>
#include <QSharedPointer>
#include "sprite_manager.h"
#include <algorithm>

class PanneauScores : public PanneauMenu
{
    Q_OBJECT
public:
    PanneauScores(QWidget* parent = nullptr);
    ~PanneauScores() = default;

protected:
    void creer() override;
    void positionner() override;
    void paintEvent(QPaintEvent* event) override;

private:
    TexteMenu* setupTexteMenu(const QString& txt);

    Bouton*  boutonRetour = nullptr;
    QFont fontPixel;
    QSharedPointer<QPixmap> imgPanneau;

    struct Ligne {
        TexteMenu* rang = nullptr;
        TexteMenu* nom = nullptr;
        TexteMenu* score = nullptr;
    };
    QList<Ligne> lignes;
    QLabel* labelTitre = nullptr;

    void rafraichirLignes();
};

#endif
