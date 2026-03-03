#include "texte_menu.h"

void TexteMenu::setAlignment(Qt::Alignment align)
{
    m_alignment = align;
    update();
}

void TexteMenu::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont f = styleEcriture.family().isEmpty() ? font() : styleEcriture;
    p.setFont(f);

    const QRect r = rect();

    const int c = int(std::ceil(largeurContour));
    const int ox = int(std::ceil(std::abs(ombrageOffset.x())));
    const int oy = int(std::ceil(std::abs(ombrageOffset.y())));

    const int padL = c + (ombrageOffset.x() < 0 ? ox : 0);
    const int padR = c + (ombrageOffset.x() > 0 ? ox : 0);
    const int padT = c + (ombrageOffset.y() < 0 ? oy : 0);
    const int padB = c + (ombrageOffset.y() > 0 ? oy : 0);

    QRect rr = r.adjusted(padL, padT, -padR, -padB);
    if (rr.width() <= 0 || rr.height() <= 0) rr = r;

    QFontMetrics fm(f);

    const int textW = fm.horizontalAdvance(texte);

    int x = rr.left();
    if (m_alignment & Qt::AlignHCenter) {
        x = rr.center().x() - textW / 2;
    }
    else if (m_alignment & Qt::AlignRight) {
        x = rr.left() + rr.width() - textW;
    }

    int y = rr.top() + fm.ascent();
    if (m_alignment & Qt::AlignVCenter) {
        y = rr.center().y() + (fm.ascent() - fm.descent()) / 2;
    }
    else if (m_alignment & Qt::AlignBottom) {
        y = rr.bottom() - fm.descent();
    }

    QPainterPath path;
    path.addText(QPointF(x, y), f, texte);

    {
        QPainterPath shadowPath = path.translated(ombrageOffset);
        p.setPen(Qt::NoPen);
        p.setBrush(ombrage);
        p.drawPath(shadowPath);
    }

    {
        QPen pen(contour, largeurContour, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawPath(path);
    }

    {
        p.setPen(Qt::NoPen);
        p.setBrush(remplissage);
        p.drawPath(path);
    }
}