#ifndef ECRAN_JEU_H
#define ECRAN_JEU_H

#include <QDir>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPropertyAnimation>
#include <QShowEvent>
#include <QResizeEvent>
#include <QPixmap>

#include "fade_overlay.h"
#include "jeu.h"

class EcranJeu : public QWidget
{
    Q_OBJECT

public:
    explicit EcranJeu(QWidget* parent = nullptr);
    ~EcranJeu();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;

private:
    void tick();

    QTimer timer;
    QElapsedTimer elapsed;

    FadeOverlay* overlay = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;

    QPixmap background;

    Jeu* jeu = nullptr;
};

#endif