#ifndef TEXTE_MENU_H
#define TEXTE_MENU_H

#include <Qt>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPainterPath>

class TexteMenu : public QWidget
{
    Q_OBJECT
public:
    TexteMenu(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const { return m_alignment; }

    void setTexte(const QString& t) { texte = t; update(); }
    QString getTexte() const { return texte; }

    void setFont(const QFont& f) { styleEcriture = f; updateGeometry(); update(); }

    void setRemplissage(const QColor& c) { remplissage = c; update(); }
    void setContour(const QColor& c) { contour = c; update(); }
    void setOmbrage(const QColor& c) { ombrage = c; update(); }

    void setLargeurContour(float w) { largeurContour = w; updateGeometry(); update(); }
    void setOmbrageOffset(QPointF p) { ombrageOffset = p; updateGeometry(); update(); }

protected:
    QSize sizeHint() const override { return QSize(200, 32); }

    void paintEvent(QPaintEvent*) override;

private:
    QString texte;
    QFont   styleEcriture;
    QColor  remplissage = QColor("#FFE066");
    QColor  contour = QColor("#B11B1B");
    QColor  ombrage = QColor(0, 0, 0, 200);
    float   largeurContour = 4.0f;
    QPointF ombrageOffset = QPointF(3.0, 3.0);
    Qt::Alignment m_alignment = Qt::AlignCenter | Qt::AlignVCenter;
};

#endif