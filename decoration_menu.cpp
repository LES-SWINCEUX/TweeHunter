#include "decoration_menu.h"

DecorationMenu::DecorationMenu(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    tempsEcouleAnimation.start();

    connect(&timerAnimation, &QTimer::timeout, this, [this]() { update(); });
    setFPS(fps);
    setNombreImages(6);
}

void DecorationMenu::setFPS(int fps)
{
    this->fps = std::max(1, fps);
    timerAnimation.setInterval(1000 / fps);

    if (timerAnimation.isActive()) {
        return;
    }

    timerAnimation.start();
}

void DecorationMenu::setCycle(int ms)
{
    cycle = std::max(1, ms);
    for (int i = 0; i < sprites.length(); i++) {
        sprites[i].setCycle(cycle);
    }
    update();
}

void DecorationMenu::setNombreImages(int nombre)
{
    sprites.resize(std::max(0, nombre));
    redessiner();
    update();
}

void DecorationMenu::setSprite(const QString& cheminSprite)
{
    QSharedPointer<QPixmap> pix = SpriteManager::instance().getPixmap(cheminSprite.startsWith(":/") ? cheminSprite : (QDir::currentPath() + cheminSprite));
    if (!pix || pix->isNull()) {
        std::cout << "MENU_DECORATION::impossible de charger ->" << cheminSprite.toStdString() << std::endl;
        return;
    }

    SpriteSheet sheet(pix, colonnes, lignes);

    for (int i = 0; i < sprites.length(); i++) {
        sprites[i].setSprite(sheet);
        sprites[i].setCycle(cycle);
    }

    redessiner();
    update();
}


void DecorationMenu::setZones(const QVector<QRectF>& zones)
{
    zonesN = zones;
    update();
}

void DecorationMenu::redessiner()
{
    for (int i = 0; i < sprites.size(); ++i) {
        sprites[i].setDecalageImage(i * 2);
        sprites[i].setCycle(cycle);
    }
}

QRect DecorationMenu::zone(const QRectF& zone) const
{
    return QRect(
        int(zone.x() * width()),
        int(zone.y() * height()),
        int(zone.width() * width()),
        int(zone.height() * height())
    );
}

void DecorationMenu::paintEvent(QPaintEvent*)
{
    if (sprites.isEmpty() || zonesN.isEmpty()) {
        return;
    }

    QPainter p(this);
    const qint64 ms = tempsEcouleAnimation.elapsed();

    const int count = std::min<int>(sprites.size(), zonesN.size());

    for (int i = 0; i < count; ++i)
    {
        QRect target = zone(zonesN[i]);

        float temps = float(ms) / 1000.0f;
        int bonds = int(6.0f * qSin((temps * 2.0f) + float(i) * 1.3f));
        target.translate(0, bonds);

        sprites[i].dessiner(p, target, ms, true);
    }
}