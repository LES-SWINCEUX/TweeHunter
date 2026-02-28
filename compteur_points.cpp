#include "compteur_points.h"

CompteurPoints::CompteurPoints(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    connect(&timerAnimation, &QTimer::timeout, this, [this]() {
        if (points == pointsCible) { 
            timerAnimation.stop();
            return;
        }

        if (points < pointsCible) {
            points = std::min(points + tempsParPoint, pointsCible);
        }
        else {
            points = std::max(points - tempsParPoint, pointsCible);
        }

        update();
    });

    setStyleEcriture();
    setSpritesheet();
}

void CompteurPoints::setNombresNumeros(int count)
{
    nombreNumero = std::max(1, count);
    update();
}

void CompteurPoints::setAnimation(bool on)
{
    estAnime = on;
    if (!estAnime) {
        timerAnimation.stop();
    }
}

void CompteurPoints::setVitesseAnimation(int pointsPerTick, int tickMs)
{
    tempsParPoint = std::max(1, pointsPerTick);
    timerAnimation.setInterval(std::max(1, tickMs));
}

void CompteurPoints::setPoints(int value)
{
    value = std::max(0, value);

    if (!estAnime) {
        points = value;
        pointsCible = value;
        update();
        return;
    }

    pointsCible = value;
    if (!timerAnimation.isActive()) timerAnimation.start();
}

void CompteurPoints::setEchelle(qreal s)
{
    if (s < 0.1) {
        s = 0.1;
    }

    if (qFuzzyCompare(s, echelle)) {
        return;
    }

    echelle = s;
    recalculerTaille();
    update();
}

void CompteurPoints::recalculerTaille()
{
    if (tailleSprite.isEmpty()) {
        return;
    }

    QSize scaled(int(tailleSprite.width() * echelle),
        int(tailleSprite.height() * echelle));
    setFixedSize(scaled);
}

QString CompteurPoints::formatterPoints(int v) const
{
    QString s = QString::number(v);

    if (s.size() < nombreNumero) {
        s = QString(nombreNumero - s.size(), '0') + s;
    }
        
    return s;
}

void CompteurPoints::setSpritesheet()
{
    spriteSheet = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/compteur_points.png");

    if (spriteSheet.isNull()) {
        return;
    }

    tailleSprite = spriteSheet->size();
    recalculerTaille();
    update();
}

void CompteurPoints::setStyleEcriture()
{
    styleEcriture = QFont("Press Start 2P");
    styleEcriture.setStyleStrategy(QFont::NoAntialias);
    update();
}

void CompteurPoints::paintEvent(QPaintEvent*)
{
    if (spriteSheet.isNull()) {
        return;
    }

    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    p.setRenderHint(QPainter::Antialiasing, false);

    p.drawPixmap(rect(), *spriteSheet);

    QFont f = styleEcriture;

    int fontSize = int(height() * 0.55);

    f.setPixelSize(fontSize);
    f.setStyleStrategy(QFont::NoAntialias);

    p.setFont(f);

    QRect textRect = rect();
    int leftPadding = int(rect().width() * 0.28);
    int rightPadding = int(rect().width() * 0.08);
    textRect.adjust(leftPadding, 0, -rightPadding, 0);

    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignRight, formatterPoints(points));
}