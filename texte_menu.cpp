#include "texte_menu.h"

void TexteMenu::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing, true);

    QFont f = styleEcriture.family().isEmpty() ? font() : styleEcriture;
    p.setFont(f);

    const QRect r = rect();

    QFontMetrics fm(f);
    QRect br = fm.boundingRect(texte);

    const int x = r.center().x() - br.width() / 2;
    const int y = r.center().y() + (fm.ascent() - fm.descent()) / 2;

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