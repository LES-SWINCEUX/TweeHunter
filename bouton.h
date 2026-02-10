#ifndef BOUTTON_H
#define BOUTTON_H

#include <QWidget>
#include <QPixmap>
#include <QSharedPointer>
#include "sprite_manager.h"
#include <QDir>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <iostream>

class Bouton : public QWidget
{
    Q_OBJECT

public:
    Bouton(const QString& cheminSprite, int nombreImages = 3, QWidget* parent = nullptr);

    void setNombreImages(int nombre);
    void setEchelle(float s);
    QSize sizeHint() const override;
    QSize tailleImage() const;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    enum class Etat { Normal = 0, Selectionne = 1, Clique = 2 };

    QSharedPointer<QPixmap> sprite;
    int images = 3;
    float echelle = 1.0f;

    Etat etat = Etat::Normal;
    bool selectionne = false;
    bool clique = false;

    QRect rectangeEtat(Etat s) const;
    void updateTailleImage();
};

#endif