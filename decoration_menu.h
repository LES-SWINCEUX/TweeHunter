#ifndef DECORATION_MENU_H
#define DECORATION_MENU_H

#include "sprite.h"
#include "sprite_manager.h"
#include "spritesheet.h"

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QRectF>
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QDir>
#include <algorithm>
#include <iostream>

class DecorationMenu : public QWidget
{
    Q_OBJECT
public:
    DecorationMenu(QWidget* parent = nullptr);

    void setSprite(const QString& cheminSprite);
    void setZones(const QVector<QRectF>& zones);
    void setNombreImages(int nombre);

    void setFPS(int fps);
    void setCycle(int ms);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<QRectF> zonesN;
    QVector<Sprite> sprites;

    int colonnes = 4;
    int lignes = 3;

    QTimer timerAnimation;
    QElapsedTimer tempsEcouleAnimation;

    int fps = 30;
    int cycle = 1000;

    QRect zone(const QRectF& zone) const;
    void redessiner();
};

#endif