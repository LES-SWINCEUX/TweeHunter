#ifndef FADE_OVERLAY_H
#define FADE_OVERLAY_H

#include <QWidget>
#include <QPainter>

class FadeOverlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)

public:
    FadeOverlay(QWidget* parent = nullptr);

    int alpha() const;
    void setAlpha(int alpha);

protected:
    void paintEvent(QPaintEvent*);

private:
    int masqueAlpha = 0;
};

#endif