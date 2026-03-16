#include "compteur_balles.h"

CompteurBalles::CompteurBalles(QWidget* parent,int balle)
    : QWidget(parent)
{
    setMaxBalles(balle);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setSpriteSheet();
	
}

void CompteurBalles::setSpriteSheet()
{
    spriteSheet = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/balles.png");
    if (spriteSheet.isNull()) {
        return;
    }

    tailleFrame = QSize(spriteSheet->width() / 5, spriteSheet->height() / 2);

    recalcFrameRect();
    recalcWidgetSize();
    update();
}

void CompteurBalles::setBalles(int value)
{
    int clamped = std::clamp(value, 0, maxBalles);
    if (clamped == balles) {
        return;
    }
    balles = clamped;
    recalcFrameRect();
    update();
}

void CompteurBalles::setEchelle(float s)
{
    if (s < 0.1) {
        s = 0.1;
    }
    if (qFuzzyCompare(s, echelle)) {
        return;
    }
    echelle = s;
    recalcWidgetSize();
    update();
}

void CompteurBalles::recalcFrameRect()
{
    if (spriteSheet.isNull() || tailleFrame.isEmpty()) {
        return;
    }

    int cols = 5;
    int col = balles % cols;
    int row = balles / cols;

    rectangleFrame = QRect(col * tailleFrame.width(), row * tailleFrame.height(),
        tailleFrame.width(), tailleFrame.height());
}

void CompteurBalles::recalcWidgetSize()
{
    if (tailleFrame.isEmpty()) {
        return;
    }

    QSize scaled(int(tailleFrame.width() * echelle), int(tailleFrame .height() * echelle));
    setFixedSize(scaled);
}

void CompteurBalles::paintEvent(QPaintEvent*)
{
    if (spriteSheet.isNull()) {
        return;
    }

    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, false); // pixel perfect
    p.setRenderHint(QPainter::Antialiasing, false);

    p.drawPixmap(rect(), *spriteSheet, rectangleFrame);
}