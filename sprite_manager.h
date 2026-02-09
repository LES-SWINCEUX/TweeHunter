#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <QHash>
#include <QMutex>
#include <QPixmap>
#include <QSharedPointer>
#include <QString>
#include <QDir>
#include <QMutexLocker>
#include <QFileInfo>

class SpriteManager
{
public:
    static SpriteManager& instance();

    QSharedPointer<QPixmap> getPixmap(const QString& cheminSprite);

    void clear();

private:
    SpriteManager() = default;

    QMutex mutex;
    QHash<QString, QSharedPointer<QPixmap>> cache;
};

#endif
