#ifndef COMPTEUR_POWERUP_H
#define COMPTEUR_POWERUP_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <algorithm>
#include <QLabel>

#include "configuration_partie.h"
#include "sprite_manager.h"

class CompteurPowerUp : public QWidget
{
    Q_OBJECT
public:
    CompteurPowerUp(QWidget* parent = nullptr, PowerUpType type = PowerUpType::GRENADE, int nbInitial = 0);

    void setPowerUp(int value);
    int  getPowerUp() const { return nbRestant; }

    void setEchelle(float s);
    float getEchelle() const { return echelle; }

    QSize tailleIconBase() const { return tailleIcon; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    void chargerSpriteSheet();
    void recalcWidgetSize();

    int indexType(PowerUpType t) const;

    QSharedPointer<QPixmap> spriteSheet;
    QLabel* labelCount = nullptr;

    PowerUpType typePowerUp;

    int nbRestant  = 0;
    int nbMax = 0;

    QSize tailleIcon;

    float echelle = 1.0f;

    const int NB_COLONNES = 4;
};

#endif
