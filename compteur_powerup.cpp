#include "compteur_powerup.h"

CompteurPowerUp::CompteurPowerUp(QWidget* parent, PowerUpType type, int nbInitial)
    : QWidget(parent)
    , typePowerUp(type)
    , nbRestant(nbInitial)
    , nbMax(nbInitial)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent,      false);
    chargerSpriteSheet();

    labelCount = new QLabel(this);
    QFont font("Press Start 2P");
    font.setStyleStrategy(QFont::NoAntialias);
    labelCount->setFont(font);
    labelCount->setStyleSheet("color: white;");
    labelCount->setAttribute(Qt::WA_TranslucentBackground);

    setPowerUp(nbInitial);
}

int CompteurPowerUp::indexType(PowerUpType t) const
{
    return (int)t;
}

void CompteurPowerUp::chargerSpriteSheet()
{
    spriteSheet = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/power_ups.png");

    if (!spriteSheet || spriteSheet->isNull()) {
        return;
    }

    tailleIcon = QSize(spriteSheet->width() / NB_COLONNES, spriteSheet->height());

    recalcWidgetSize();
    update();
}

void CompteurPowerUp::setPowerUp(int value)
{
    nbRestant = std::max(0, value);
    if (labelCount) {
        labelCount->setText(QString("x%1").arg(nbRestant));
        labelCount->adjustSize();
        labelCount->show();
        recalcWidgetSize();
    }
    update();
}

void CompteurPowerUp::setEchelle(float s)
{
    if (s < 0.1f) {
        s = 0.1f;
    }

    if (qFuzzyCompare(s, echelle)) {
        return;
    }

    echelle = s;
    recalcWidgetSize();
    update();
}

void CompteurPowerUp::recalcWidgetSize()
{
    if (tailleIcon.isEmpty()) {
        return;
    }

    int iconW = int(tailleIcon.width() * echelle);
    int iconH = int(tailleIcon.height() * echelle);
    int fontSize = std::max(8, int(iconH * 0.38f));

    if (labelCount) {
        QFont font = labelCount->font();
        font.setPixelSize(fontSize);
        labelCount->setFont(font);
        labelCount->setText(QString("x%1").arg(nbRestant));
        labelCount->adjustSize();
        labelCount->move(iconW, 0);
        labelCount->setFixedHeight(iconH);
        labelCount->show();
    }

    int textW = labelCount ? labelCount->width() + int(iconH * 0.2f) : iconH;
    setFixedSize(iconW + textW, iconH);
}

void CompteurPowerUp::paintEvent(QPaintEvent*)
{
    if (!spriteSheet || spriteSheet->isNull()) {
        return;
    }

    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    p.setRenderHint(QPainter::Antialiasing, false);

    int col = (int)typePowerUp;
    int iconW = int(tailleIcon.width() * echelle);
    int iconH = int(tailleIcon.height() * echelle);

    QRect src(col * tailleIcon.width(), 0, tailleIcon.width(), tailleIcon.height());
    QRect dst(0, 0, iconW, iconH);
    p.drawPixmap(dst, *spriteSheet, src);
}
