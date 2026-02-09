#include "sprite_manager.h"

SpriteManager& SpriteManager::instance()
{
    static SpriteManager instance;
    return instance;
}

static QString normaliseCle(const QString& cheminSprite)
{
    if (cheminSprite.startsWith(":/")) {
        return cheminSprite;
    }

    QFileInfo informationsFichier(cheminSprite);
    if (informationsFichier.isAbsolute()) {
        return informationsFichier.canonicalFilePath().isEmpty() ? informationsFichier.absoluteFilePath() : informationsFichier.canonicalFilePath();
    }

    const QString cheminAbsolue = QDir::current().absoluteFilePath(cheminSprite);
    QFileInfo informationsFichierAbsolue(cheminAbsolue);
    return informationsFichierAbsolue.canonicalFilePath().isEmpty() ? informationsFichierAbsolue.absoluteFilePath() : informationsFichierAbsolue.canonicalFilePath();
}

QSharedPointer<QPixmap> SpriteManager::getPixmap(const QString& cheminSprite)
{
    const QString cle = normaliseCle(cheminSprite);

    QMutexLocker locker(&mutex);

    auto valeur = cache.find(cle);
    if (valeur != cache.end() && *valeur && !(*valeur)->isNull()) {
        return *valeur;
    }

    QSharedPointer<QPixmap> pixmap(new QPixmap(cle));
    cache.insert(cle, pixmap);
    return pixmap;
}

void SpriteManager::clear()
{
    QMutexLocker locker(&mutex);
    cache.clear();
}
