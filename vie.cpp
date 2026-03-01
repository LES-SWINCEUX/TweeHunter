#include "vie.h"

Vies::Vies(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setSpriteSheet();
}

void Vies::setSpriteSheet()
{
    QSharedPointer<QPixmap> px = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/indicateur_vie.png");
    if (px.isNull()) {
        return;
    }

    spriteSheet = px;

    tailleFrame = QSize(spriteSheet->width() / framesSpriteSheet, spriteSheet->height());

    recalculerSprite();
    update();
}

void Vies::setVies(int vies)
{
    maxVies = std::max(1, vies);
    demiVies = std::clamp(demiVies, 0, maxVies * 2);
    recalculerSprite();
    update();
}

void Vies::setDemiVies(int demiVies)
{
    int clamped = std::clamp(demiVies, 0, maxVies * 2);
    if (clamped == this->demiVies) {
        return;
    }
    this->demiVies = clamped;
    update();
}

void Vies::setEchelle(float s)
{
    if (s < 0.1) s = 0.1;
    if (qFuzzyCompare(s, echelle)) {
        return;
    }
    echelle = s;
    recalculerSprite();
    update();
}

void Vies::recalculerSprite()
{
    if (tailleFrame.isEmpty()) {
        return;
    }

    int espacement = int(espacementVies * echelle);
    int largeur = int(maxVies * tailleFrame.width() * echelle) + int((maxVies - 1) * espacement);
    int hauteur = int(tailleFrame.height() * echelle);

    setFixedSize(largeur, hauteur);
}

void Vies::paintEvent(QPaintEvent*)
{
    if (spriteSheet.isNull() || tailleFrame.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    int espacement = int(espacementVies * echelle);

    for (int i = 0; i < maxVies; ++i)
    {
        int viesRestantes = demiVies - i * 2;

        int indexFrame;

        if (viesRestantes >= 2) {
            indexFrame = 0;
        }
        else if (viesRestantes == 1) {
            indexFrame = 1;
        }
        else {
            indexFrame = 2;
        }

        QRect src(indexFrame * tailleFrame.width(), 0, tailleFrame.width(), tailleFrame.height());

        int x = int(i * (tailleFrame.width() * echelle) + i * espacement);
        QRect dst(x, 0, int(tailleFrame.width() * echelle), int(tailleFrame.height() * echelle));

        painter.drawPixmap(dst, *spriteSheet, src);
    }
}