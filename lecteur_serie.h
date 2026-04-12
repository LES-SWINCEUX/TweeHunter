#ifndef LECTEUR_SERIE_H
#define LECTEUR_SERIE_H

#include <QObject>
#include <QThread>
#include "Touches.h"

class LecteurSerie : public QObject
{
    Q_OBJECT

public:
    explicit LecteurSerie(Touches* touches, QObject* parent = nullptr);
    void arreter() { actif = false; }

public slots:
    void demarrer();

signals:
    void gachetteChangee(bool etat);
    void reloadChange(bool etat);
    void accelerometreChange(bool etat);
    void joystickChange(int x, int y);
    void encodeurChange(int valeur);

private:
    Touches* touches;
    bool actif = true;
};

#endif
