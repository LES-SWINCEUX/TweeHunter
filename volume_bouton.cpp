#include "volume_bouton.h"

VolumeBouton::VolumeBouton(const QString& cheminBouton, GestionnaireAudio* gestionnaireAudio, AudioMode mode, QWidget* parent)
    : Bouton(cheminBouton, 3, parent)
{
    this->gestionnaireAudio = gestionnaireAudio;
    this->mode = mode;
    // Petit comportement pratique: cliquer fait cycler le volume.
    connect(this, &Bouton::clicked, this, [this]() {
        this->onButtonClick();
    });
}

void VolumeBouton::setVolume(float v)
{
    if (this->mode == MUSIQUE) {
        this->gestionnaireAudio->setMusicVolume(v);
        return;
    }
    this->gestionnaireAudio->setSfxVolume(v);
}

void VolumeBouton::onButtonClick() {
    float v = 0.0f;
    if (mode == MUSIQUE) {
        v = this->gestionnaireAudio->getMusicVolume() + (1.0f / volumeBars);
    }
    else {
        v = this->gestionnaireAudio->getSfxVolume() + (1.0f / volumeBars);
    }
    if (v > 0.9f && v < 1.0f) {
        v = 1.0f;
    }
    else if (v > 1.001f) {
        v = 0.0f;
    }
    setVolume(v);
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
        const float v = parseVolume(mode == MUSIQUE ? this->gestionnaireAudio->getMusicVolume() : this->gestionnaireAudio->getSfxVolume());
        const int filled = int(qRound(v * volumeBars));

        const int gap = qMax(3, int(barsRect.width() * 0.03));
        const int barW = qMax(6, (barsRect.width() - gap * (volumeBars - 1)) / volumeBars);
        const int maxH = barsRect.height();
        const int baseY = barsRect.bottom();

        for (int i = 0; i < volumeBars; ++i) {
            float t = float(i + 1) / float(volumeBars);
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

float VolumeBouton::parseVolume(float volume) {
    if (volume <= 0.0f) {
        return 0.0f;
    }
    if (volume >= 1.0f) {
        return 1.0f;
    }
    return volume;
}
