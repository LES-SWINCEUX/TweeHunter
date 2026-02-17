#include "volume_bouton.h"

#include <QPainterPath>
#include <QFontMetrics>

static float clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

VolumeBouton::VolumeBouton(const QString& cheminBouton, QAudioOutput* mixeurVolume, float volumeMax, QWidget* parent)
    : Bouton(cheminBouton, 3, parent)
{
    this->volumeMax = volumeMax;
    this->mixeurVolume = mixeurVolume;
    this->volume_ = mixeurVolume->volume() / this->volumeMax;
    // Petit comportement pratique: cliquer fait cycler le volume.
    connect(this, &Bouton::clicked, this, [this]() {
        float v = volume_ + 0.14285f;
        if (v > 0.9f && v < 1.0f) {
            v = 1.0f;
        }
        else if (v > 1.001f) {
            v = 0.0f;
        }
        setVolume(v);
    });
}

void VolumeBouton::setVolume(float v)
{
    volume_ = clamp01(v);
    update();
    if (this->mixeurVolume == nullptr) {
        return;
    }
    mixeurVolume->setVolume(volume_ * this->volumeMax);
}

void VolumeBouton::paintEvent(QPaintEvent* e)
{
    // Dessine le sprite (normal/hover/click)
    Bouton::paintEvent(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Zone intérieure "safe" (on évite le cadre doré)
    const int padX = qMax(8, int(width() * 0.2));
    const int padY = qMax(6, int(height() * 0.3));
    QRect inner = rect().adjusted(padX, padY, -padX, -padY);
    if (inner.width() <= 10 || inner.height() <= 10) return;

    // Couleurs (style rétro)
    const QColor gold(255, 210, 80);
    const QColor goldDark(180, 120, 10);
    const QColor white(235, 235, 235);
    const QColor shadow(0, 0, 0, 90);

    // Découpe inner en 2: icone à gauche, barres à droite
    const int iconW = qMax(40, int(inner.width() * 0.22));
    QRect iconRect(inner.left(), inner.top(), iconW, inner.height());
    QRect barsRect(inner.left() + iconW + qMax(8, int(inner.width() * 0.03)), inner.top(),
                   inner.width() - iconW - qMax(8, int(inner.width() * 0.03)), inner.height());


    // -------- Barres de volume --------
    {
        const int bars = 7;
        const float v = clamp01(volume_);
        const int filled = int(qRound(v * bars));

        const int gap = qMax(3, int(barsRect.width() * 0.03));
        const int barW = qMax(6, (barsRect.width() - gap * (bars - 1)) / bars);
        const int maxH = barsRect.height();
        const int baseY = barsRect.bottom();

        for (int i = 0; i < bars; ++i) {
            float t = float(i + 1) / float(bars);
            int h = qMax(6, int(maxH * t));
            int x = barsRect.left() + i * (barW + gap);
            QRect r(x, baseY - h, barW, h);

            // contour
            p.setPen(QPen(goldDark, 2));
            p.setBrush(QColor(255, 220, 120, (i < filled) ? 255 : 80));
            p.drawRoundedRect(r, 2, 2);

            // petit "shine" si rempli
            if (i < filled) {
                QRect shine = r.adjusted(2, 2, -barW/2, -h/2);
                p.setPen(Qt::NoPen);
                p.setBrush(QColor(255, 255, 255, 60));
                p.drawRoundedRect(shine, 2, 2);
            }
        }
    }
}
