#include "animation_clock.h"

AnimationClock& AnimationClock::instance()
{
    static AnimationClock inst;
    return inst;
}

AnimationClock::AnimationClock(QObject* parent) : QObject(parent)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &AnimationClock::onTimeout);
}

void AnimationClock::start(int fps)
{
    setFps(fps);
    if (!m_elapsed.isValid()) {
        m_elapsed.start();
    }
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void AnimationClock::stop()
{
    m_timer.stop();
}

void AnimationClock::setFps(int fps)
{
    m_fps = std::clamp(fps, 1, 240);
    const int intervalMs = std::max(1, 1000 / m_fps);
    m_timer.setInterval(intervalMs);
}

qint64 AnimationClock::nowMs() const
{
    return m_elapsed.isValid() ? m_elapsed.elapsed() : 0;
}

void AnimationClock::onTimeout()
{
    emit tick(nowMs());
}
