#include "gestionnaire_audio.h"
#include <QFileInfo>

GestionnaireAudio::GestionnaireAudio(QObject* parent) : QObject(parent)
{
    musicOutput = new QAudioOutput(this);
    musicPlayer = new QMediaPlayer(this);

    musicPlayer->setAudioOutput(musicOutput);
    musicOutput->setVolume(this->MAX_MUSIC_VOLUME);
    musicVolume = 1.0f;
    sfxVolume = 1.0f;

    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged,
        this, &GestionnaireAudio::onMediaFinished);

    chargerParametres();
}

void GestionnaireAudio::setPlaylist(const QStringList& musics)
{
    playlist = musics;
    index = 0;

    std::shuffle(playlist.begin(), playlist.end(), std::mt19937{std::random_device{}()});
}

void GestionnaireAudio::playMusic()
{
    if (playlist.isEmpty()) {
        return;
    }

    musicPlayer->setSource(QUrl(playlist[index]));
    musicPlayer->play();
}

void GestionnaireAudio::nextMusic()
{
    if (playlist.isEmpty()) {
        return;
    }

    index = (index + 1) % playlist.size();

    if (index == 0) {
        QString derniere = playlist.last();
        do {
            std::shuffle(playlist.begin(), playlist.end(), std::mt19937{std::random_device{}()});
        } while (playlist.size() > 1 && playlist.first() == derniere);
    }

    playMusic();
}

void GestionnaireAudio::setMusicVolume(float v)
{
    float parsedVolume = v;
    if (v >= 1.0f) {
        parsedVolume = 1.0f;
    }
    else if (v <= 0.0f) {
        parsedVolume = 0.0f;
    }
    this->musicVolume = v;
    musicOutput->setVolume(v * this->MAX_MUSIC_VOLUME);
    sauvegarderParametres();
}

float GestionnaireAudio::getMusicVolume() const {
    return musicOutput->volume() / this->MAX_MUSIC_VOLUME;
}

float GestionnaireAudio::getMusicVolumeSetting() const {
    return this->musicVolume;
}

float GestionnaireAudio::getMaxMusicVolume() const {
    return this->MAX_MUSIC_VOLUME;
}

void GestionnaireAudio::stopMusic()
{
    musicPlayer->stop();
}

void GestionnaireAudio::clearPlaylist()
{
    playlist.clear();
    index = 0;
}

void GestionnaireAudio::stopAndClearMusic()
{
    musicPlayer->stop();
    musicPlayer->setSource(QUrl());
    playlist.clear();
    index = 0;
}

void GestionnaireAudio::onMediaFinished(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
        nextMusic();
}

void GestionnaireAudio::addSfx(const QString& name, const QString& path, int voices)
{
    voices = std::max(1, voices);

    SfxPool pool;
    pool.players.reserve(voices);

    QUrl url = QUrl::fromLocalFile(path);

    for (int i = 0; i < voices; ++i) {
        auto* effect = new QSoundEffect(this);

        effect->setSource(url);
        effect->setVolume(this->sfxVolume * this->MAX_SFX_VOLUME);

        pool.players.push_back(effect);
    }

    sfx.insert(name, pool);
}

void GestionnaireAudio::playSfx(const QString& name)
{
    if (!sfx.contains(name))
        return;

    SfxPool& pool = sfx[name];

    QSoundEffect* p = pool.players[pool.nextIndex];
    pool.nextIndex = (pool.nextIndex + 1) % pool.players.size();

    p->play();
}

void GestionnaireAudio::setSfxVolume(float v)
{
    float parsedVolume = v;

    if (v >= 1.0f) {
        parsedVolume = 1.0f;
    }
    else if (v <= 0.0f) {
        parsedVolume = 0.0f;
    }

    sfxVolume = parsedVolume;

    for (auto it = sfx.begin(); it != sfx.end(); ++it) {
        SfxPool& pool = it.value();
        for (QSoundEffect* effect : pool.players) {
            effect->setVolume(parsedVolume * this->MAX_SFX_VOLUME);
        }
    }

    sauvegarderParametres();
}

float GestionnaireAudio::getSfxVolume() const {
    return this->sfxVolume;
}

float GestionnaireAudio::getSfxVolumeSetting() const {
    return this->sfxVolume * this->MAX_SFX_VOLUME;
}

float GestionnaireAudio::getMaxSfxVolume() const {
    return this->MAX_SFX_VOLUME;
}

void GestionnaireAudio::setMusicVolumeAnimation(float v) {
    float parsedVolume = v;
    if (v >= 1.0f) {
        parsedVolume = 1.0f;
    }
    else if (v <= 0.0f) {
        parsedVolume = 0.0f;
    }
    musicOutput->setVolume(v * this->MAX_MUSIC_VOLUME);
}

void GestionnaireAudio::sauvegarderParametres() const
{
    QSettings settings("MonJeu", "TweeHunter");
    settings.setValue("audio/musicVolume", this->musicVolume);
    settings.setValue("audio/sfxVolume",   this->sfxVolume);
}

void GestionnaireAudio::chargerParametres()
{
    QSettings settings("MonJeu", "TweeHunter");
    float music = settings.value("audio/musicVolume", 1.0f).toFloat();
    float sfx   = settings.value("audio/sfxVolume",   1.0f).toFloat();
    setMusicVolume(music);
    setSfxVolume(sfx);
}
