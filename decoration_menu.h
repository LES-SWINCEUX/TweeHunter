#ifndef DECORATION_MENU_H
#define DECORATION_MENU_H

#include "sprite.h"
#include "animation_clock.h"

#include <QWidget>
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

    void setSprite(const QString& qrcPath);
    void setZones(const QVector<QRectF>& zones);
    void setNombreDecorations(int n);

    void setFPS(int fps);
    void setCycle(int ms);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<QRectF> zonesN;
    QVector<Sprite> sprites;

    int colonnes = 4;
    int lignes = 3;

    int fps = 30;
    int cycle = 1000;

    qint64 m_timeMs = 0;

    QRect zone(const QRectF& zn) const;
    void redessiner();
};

#endif