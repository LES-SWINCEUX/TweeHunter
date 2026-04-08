#include "bouton_options.h"

struct PaletteBouton
{
    QColor top;
    QColor bottom;
    QColor border;
    QColor inner;
};

PaletteBouton palettePourTheme(BoutonOptions::Theme theme)
{
    switch (theme) {
    case BoutonOptions::Theme::Rouge:
        return {
            QColor(150, 45, 8),
            QColor(88, 18, 3),
            QColor(120, 60, 10),
            QColor(220, 120, 40)
        };

    case BoutonOptions::Theme::Vert:
        return {
            QColor(120, 170, 25),
            QColor(50, 95, 10),
            QColor(90, 120, 20),
            QColor(180, 230, 70)
        };

    case BoutonOptions::Theme::Bleu:
        return {
            QColor(55, 95, 210),
            QColor(25, 45, 130),
            QColor(60, 85, 170),
            QColor(120, 170, 255)
        };
    }

    return {
        QColor(150, 45, 8),
        QColor(88, 18, 3),
        QColor(120, 60, 10),
        QColor(220, 120, 40)
    };
}

BoutonOptions::BoutonOptions(const QString& titre,
    const QString& sousTitre,
    Theme theme,
    QWidget* parent)
    : QPushButton(parent),
    m_titre(titre),
    m_sousTitre(sousTitre),
    m_theme(theme)
{
    setCursor(Qt::PointingHandCursor);
    setCheckable(false);
    setFocusPolicy(Qt::StrongFocus);
    setFlat(true);
    setMinimumHeight(48);
}

void BoutonOptions::setHoverActif(bool actif)
{
    m_hoverActif = actif;
    update();
}

void BoutonOptions::setSelected(bool selected)
{
    if (m_selected == selected) {
        return;
    }

    m_selected = selected;
    update();
}

void BoutonOptions::setTitre(const QString& titre)
{
    m_titre = titre;
    update();
}

void BoutonOptions::setSousTitre(const QString& sousTitre)
{
    m_sousTitre = sousTitre;
    update();
}

void BoutonOptions::setTheme(Theme theme)
{
    m_theme = theme;
    update();
}

void BoutonOptions::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QRectF r = rect().adjusted(3, 3, -3, -3);
    if (r.width() <= 0 || r.height() <= 0) {
        return;
    }

    const bool hover = m_hoverActif && (underMouse() || hasFocus());
    const bool selected = m_selected;

    PaletteBouton pal = palettePourTheme(m_theme);

    QColor top = pal.top;
    QColor bottom = pal.bottom;

    if (isDown()) {
        top = top.darker(125);
        bottom = bottom.darker(130);
    }
    else if (hover) {
        top = top.lighter(125);
        bottom = bottom.lighter(118);
    }

    QColor borderOuter = QColor(95, 45, 5);
    QColor borderMain = QColor(180, 110, 20);

    if (selected) {
        borderOuter = QColor(255, 245, 170);
        borderMain = QColor(255, 220, 90);
    }
    else if (hover) {
        borderOuter = QColor(255, 225, 120);
        borderMain = QColor(255, 200, 70);
    }

    // Glow hover / focus plus visible
    if (hover) {
        QRectF glowRect = r.adjusted(-6, -6, 6, 6);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 220, 90, selected ? 70 : 55));
        p.drawRoundedRect(glowRect, 16, 16);
    }

    // Ombre
    QRectF shadowRect = r.translated(0, 3);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(45, 10, 0, hover ? 70 : 90));
    p.drawRoundedRect(shadowRect, 12, 12);

    // Fond principal
    QLinearGradient grad(r.topLeft(), r.bottomLeft());
    grad.setColorAt(0.0, top);
    grad.setColorAt(1.0, bottom);

    p.setBrush(grad);
    p.setPen(QPen(borderOuter, hover || selected ? 6 : 5));
    p.drawRoundedRect(r, 12, 12);

    // Deuxième outline légère, mais pas de inline brun
    QRectF rim = r.adjusted(3, 3, -3, -3);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(borderMain, hover || selected ? 3 : 2));
    p.drawRoundedRect(rim, 10, 10);

    // Reflet intérieur
    QRectF shine = r.adjusted(7, 7, -7, -r.height() * 0.52);
    QLinearGradient shineGrad(shine.topLeft(), shine.bottomLeft());
    shineGrad.setColorAt(0.0, QColor(255, 255, 255, hover ? 55 : 35));
    shineGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.setPen(Qt::NoPen);
    p.setBrush(shineGrad);
    p.drawRoundedRect(shine, 8, 8);

    const bool aSousTitre = !m_sousTitre.trimmed().isEmpty();

    QRect titreRect;
    QFont ft = font();
    ft.setBold(true);
    ft.setItalic(true);

    if (aSousTitre) {
        ft.setPixelSize(qMax(16, int(height() * 0.34)));
        titreRect = rect().adjusted(
            10,
            int(height() * 0.08),
            -10,
            -int(height() * 0.36)
        );
    }
    else {
        ft.setPixelSize(qMax(18, int(height() * 0.42)));
        titreRect = rect().adjusted(10, 0, -10, 0);
    }

    p.setFont(ft);
    p.setPen(QColor(60, 25, 0));
    p.drawText(titreRect.translated(2, 2), Qt::AlignCenter, m_titre);

    p.setPen(hover || selected ? QColor(255, 250, 205) : QColor(255, 245, 175));
    p.drawText(titreRect, Qt::AlignCenter, m_titre);

    if (aSousTitre) {
        QRect sousTitreRect = rect().adjusted(
            8,
            int(height() * 0.58),
            -8,
            -6
        );

        QFont fs = font();
        fs.setBold(true);
        fs.setPixelSize(qMax(8, int(height() * 0.15)));
        p.setFont(fs);

        p.setPen(QColor(30, 20, 10));
        p.drawText(sousTitreRect.translated(1, 1), Qt::AlignHCenter | Qt::AlignTop, m_sousTitre);

        p.setPen(hover || selected ? QColor(255, 250, 220) : QColor(245, 245, 230));
        p.drawText(sousTitreRect, Qt::AlignHCenter | Qt::AlignTop, m_sousTitre);
    }
}