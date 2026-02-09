#include "decoration_menu.h"

DecorationMenu::DecorationMenu(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    connect(&AnimationClock::instance(), &AnimationClock::tick, this, [this](qint64 ms) {
        m_timeMs = ms;
        update();
    });
    setFPS(fps);
    setNombreDecorations(6);
}

void DecorationMenu::setFPS(int fps)
{
    this->fps = std::max(1, fps);
    AnimationClock::instance().start(this->fps);
}

void DecorationMenu::setCycle(int ms)
{
    cycle = std::max(1, ms);
    for (int i = 0; i < sprites.length(); i++) {
        sprites[i].setCycle(cycle);
    }
    update();
}

void DecorationMenu::setNombreDecorations(int n)
{
    sprites.resize(std::max(0, n));
    redessiner();
    update();
}

void DecorationMenu::setSprite(const QString& qrcPath)
{
    QPixmap sheet(QDir::currentPath() + qrcPath);
    if (sheet.isNull()) {
        std::cout << "MENU_DECORATION::impossible de charger ->" << qrcPath.toStdString() << std::endl;
        return;
    }

    for (int i = 0; i < sprites.length(); i++) {
        sprites[i].setSprite(sheet, colonnes, lignes);
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

QRect DecorationMenu::zone(const QRectF& zn) const
{
    return QRect(
        int(zn.x() * width()),
        int(zn.y() * height()),
        int(zn.width() * width()),
        int(zn.height() * height())
    );
}

void DecorationMenu::paintEvent(QPaintEvent*)
{
    if (sprites.isEmpty() || zonesN.isEmpty()) {
        return;
    }

    QPainter p(this);
    const qint64 ms = m_timeMs;

    const int count = std::min<int>(sprites.size(), zonesN.size());

    for (int i = 0; i < count; ++i)
    {
        QRect target = zone(zonesN[i]);

        float t = float(ms) / 1000.0f;
        int bob = int(6.0f * qSin((t * 2.0f) + float(i) * 1.3f));
        target.translate(0, bob);

        sprites[i].dessiner(p, target, ms, true);
    }
}