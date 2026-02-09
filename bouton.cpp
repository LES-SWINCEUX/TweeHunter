#include "bouton.h"

Bouton::Bouton(const QString& spritePath, int frameCount, QWidget* parent) : 
    QWidget(parent),
    sprite(QDir::currentPath() + spritePath),
    images(frameCount)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    if (sprite.isNull()) {
        std::cout << "BOUTON::impossible de charger: " << QDir::currentPath().toStdString() << spritePath.toStdString() << std::endl;
    }

    updateTailleImage();
}

void Bouton::setNombreImages(int count)
{
    images = std::max(1, count);
    updateTailleImage();
    update();
}

void Bouton::setEchelle(float s)
{
    echelle = std::max(0.1f, s);
    updateTailleImage();
    update();
}

QSize Bouton::tailleImage() const
{
    if (sprite.isNull() || images <= 0) {
        return QSize(0, 0);
    }
    return QSize(sprite.width() / images, sprite.height());
}

void Bouton::updateTailleImage()
{
    const QSize tailleImage = this->tailleImage();
    if (tailleImage.isValid()) {
        setFixedSize(int(tailleImage.width() * echelle), int(tailleImage.height() * echelle));
    }
}

QSize Bouton::sizeHint() const
{
    const QSize tailleImage = this->tailleImage();

    if (!tailleImage.isValid()) {
        return QWidget::sizeHint();
    }

    return QSize(int(tailleImage.width() * echelle), int(tailleImage.height() * echelle));
}

QRect Bouton::rectangeEtat(Etat s) const
{
    const QSize tailleImage = this->tailleImage();
    int idx = int(s);
    idx = std::clamp(idx, 0, images - 1);
    return QRect(idx * tailleImage.width(), 0, tailleImage.width(), tailleImage.height());
}

void Bouton::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    if (sprite.isNull() || images <= 0) {
        return;
    }

    Etat etat = Etat::Normal;
    if (clique) {
        etat = Etat::Clique;
    }
    else if (selectionne) {
        etat = Etat::Selectionne;
    }

    const QRect rectangleBouton = rectangeEtat(etat);
    p.drawPixmap(rect(), sprite, rectangleBouton);
}

void Bouton::enterEvent(QEnterEvent*)
{
    selectionne = true;
    update();
}

void Bouton::leaveEvent(QEvent*)
{
    selectionne = false;
    clique = false;
    update();
}

void Bouton::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        clique = true;
        update();
    }
}

void Bouton::mouseReleaseEvent(QMouseEvent* e)
{
    if (clique && e->button() == Qt::LeftButton) {
        clique = false;

        if (rect().contains(e->position().toPoint())) {
            emit clicked();
        }
        update();
    }
}