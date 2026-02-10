#include "fade_overlay.h"

FadeOverlay::FadeOverlay(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    hide();
}

int FadeOverlay::alpha() const { 
    return masqueAlpha;
}

void FadeOverlay::setAlpha(int alpha) { 
    masqueAlpha = alpha;
    update(); 
}

void FadeOverlay::paintEvent(QPaintEvent*) {
    if (masqueAlpha <= 0) {
        return;
    }
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, masqueAlpha));
}