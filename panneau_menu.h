#ifndef PANNEAU_MENU_H
#define PANNEAU_MENU_H

#include <QResizeEvent>
#include <QWidget>
#include <QDir>
#include <QList>

#include "bouton.h"
#include "Touches.h"

class PanneauMenu : public QWidget
{
    Q_OBJECT
public:
    PanneauMenu(QWidget* parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setMouseTracking(true);
    }

    virtual ~PanneauMenu() = default;

    void naviguerHaut();
    void naviguerBas();
    void confirmer();
    void reinitialiserFocus();
    void focusSurIndex(int index);
    virtual QList<Bouton*> boutonsNavigables() const { return {}; }

    void connecterTouches(Touches* touches)
    {
        if (!touches) {
            return;
        }

        connect(touches, &Touches::naviguerHaut, this, &PanneauMenu::naviguerHaut);
        connect(touches, &Touches::naviguerBas, this, &PanneauMenu::naviguerBas);
        connect(touches, &Touches::naviguerConfirmer, this, &PanneauMenu::confirmer);
    }

signals:
    void demanderJouer();
    void demanderScores();
    void demanderOptions();
    void demanderQuitter();
    void demanderRetour();
    void demanderRetourOptions();

protected:
    void initialiserPanneau()
    {
        creer();
        positionner();
    }

    virtual void creer() = 0;
    virtual void positionner() = 0;

    void resizeEvent(QResizeEvent* e) override
    {
        QWidget::resizeEvent(e);
        positionner();
    }

private:
    void appliquerFocus(int nouvelIndex);

    int indexFocus = -1;
};

#endif