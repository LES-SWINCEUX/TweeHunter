#ifndef ARRIERE_PLAN_H
#define ARRIERE_PLAN_H

#include <QPixmap>
#include <QSharedPointer>
#include <QPainter>
#include <QSize>
#include <algorithm>

class ArrierePlan
{
    public:
        ArrierePlan() = default;
        ArrierePlan(const QSharedPointer<QPixmap>& sprite) : sprite(sprite) {}
    
        void setSprite(const QSharedPointer<QPixmap>& s) { sprite = s; cache = QPixmap(); }
    
        enum class AncrageV { Haut, Centre, Bas };
    
        void mettreAJour(const QSize& taille, AncrageV ancrageV = AncrageV::Centre)
        {
            if (!sprite || sprite->isNull() || taille.isEmpty()) {
                cache = QPixmap();
                return;
            }
            QPixmap scaled = sprite->scaled(taille, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            int ox = std::max(0, (scaled.width()  - taille.width())  / 2);
            int oy = 0;
            switch (ancrageV) {
                case AncrageV::Haut: 
                    oy = 0; 
                    break;
                case AncrageV::Centre: 
                    oy = std::max(0, (scaled.height() - taille.height()) / 2); 
                    break;
                case AncrageV::Bas: 
                    oy = std::max(0, scaled.height() - taille.height());
                    break;
            }
            cache = scaled.copy(QRect(ox, oy, taille.width(), taille.height()));
        }
    
        void dessiner(QPainter& painter) const
        {
            if (!cache.isNull()) {
                painter.drawPixmap(0, 0, cache);
            } else if (sprite && !sprite->isNull()) {
                painter.drawPixmap(QRect(QPoint(0, 0), cache.size()), *sprite);
            }
        }
    
        bool estValide() const { return sprite && !sprite->isNull(); }
    
    private:
        QSharedPointer<QPixmap> sprite;
        QPixmap cache;
};

#endif
