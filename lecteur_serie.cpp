#include "lecteur_serie.h"

LecteurSerie::LecteurSerie(Touches* touches, QObject* parent)
    : QObject(parent), touches(touches) {
}

void LecteurSerie::demarrer()
{
    while (actif) {
        touches->lirePerso();

        emit gachetteChangee(touches->getGachette());
        emit reloadChange(touches->getReload());
        emit accelerometreChange(touches->getAccelerometre());
        emit joystickChange(touches->getxPerso(), touches->getyPerso());

        int enc = touches->useLastEncodeur();
        if (enc != 0) {
            emit encodeurChange(enc);
        }

        QThread::msleep(8);
    }
}