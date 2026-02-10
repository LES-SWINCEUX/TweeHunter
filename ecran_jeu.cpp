#include "ecran_jeu.h"

#include <QPainter>
#include <algorithm>

EcranJeu::EcranJeu(QWidget* parent)
    : QWidget(parent)
{
    background = QPixmap(QDir::currentPath() + "/images/jeu/background.png");
    setAttribute(Qt::WA_OpaquePaintEvent);
    elapsed.start();

    timer.setInterval(1000 / 60);
    connect(&timer, &QTimer::timeout, this, [this]() {
        tick();
        });
    timer.start();

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();

    fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeInAnim->setDuration(1000);
    fadeInAnim->setStartValue(255);
    fadeInAnim->setEndValue(0);

    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        overlay->hide();
        });
}

void EcranJeu::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);

    if (overlay && fadeInAnim)
    {
        overlay->setGeometry(rect());
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();

        fadeInAnim->stop();
        fadeInAnim->start();
    }
}

void EcranJeu::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (overlay)
    {
        overlay->setGeometry(rect());
        overlay->raise();
    }
}

void EcranJeu::tick()
{
    update();
}

void EcranJeu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (!background.isNull()) {
        painter.drawPixmap(rect(), background);
    }
    else {
        painter.fillRect(rect(), Qt::black);
    }
}