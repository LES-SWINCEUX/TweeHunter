#ifndef ANIMATION_CLOCK_H
#define ANIMATION_CLOCK_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <algorithm>

class AnimationClock final : public QObject
{
    Q_OBJECT
public:
    static AnimationClock& instance();

    void start(int fps = 60);
    void stop();
    void setFps(int fps);
    int fps() const { return m_fps; }

    qint64 nowMs() const;

signals:
    void tick(qint64 ms);

private:
    explicit AnimationClock(QObject* parent = nullptr);

private slots:
    void onTimeout();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsed;
    int m_fps = 60;
};

#endif
