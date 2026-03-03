#include "gestionnaire_audio.h"
#include <QFileInfo>

GestionnaireAudio::GestionnaireAudio(QObject* parent) : QObject(parent)
{
    musicOutput = new QAudioOutput(this);
    musicPlayer = new QMediaPlayer(this);

    musicPlayer->setAudioOutput(musicOutput);
    musicOutput->setVolume(this->maxMusicVolume);
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
    musicOutput->setVolume(v * this->maxMusicVolume);
    sauvegarderParametres();
}

float GestionnaireAudio::getMusicVolume() const {
    return musicOutput->volume() / this->maxMusicVolume;
}

float GestionnaireAudio::getMusicVolumeSetting() const {
    return this->musicVolume;
}

float GestionnaireAudio::getMaxMusicVolume() const {
    return this->maxMusicVolume;
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
    pool.outputs.reserve(voices);

    const QUrl url = QUrl::fromLocalFile(path);

    for (int i = 0; i < voices; ++i) {
        auto* out = new QAudioOutput(this);
        out->setVolume(this->sfxVolume * this->maxSfxVolume);

        auto* p = new QMediaPlayer(this);
        p->setAudioOutput(out);
        p->setSource(url);

        pool.outputs.push_back(out);
        pool.players.push_back(p);
    }

    sfx.insert(name, pool);
}

void GestionnaireAudio::playSfx(const QString& name)
{
    if (!sfx.contains(name))
        return;

    SfxPool& pool = sfx[name];

    QMediaPlayer* p = pool.players[pool.nextIndex];
    pool.nextIndex = (pool.nextIndex + 1) % pool.players.size();

    // Important: remettre au début sans couper les autres instances
    p->setPosition(0);
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
        for (auto* out : pool.outputs) {
            out->setVolume(parsedVolume * this->maxSfxVolume);
        }
    }

    sauvegarderParametres();
}

float GestionnaireAudio::getSfxVolume() const {
    return this->sfxVolume;
}

float GestionnaireAudio::getSfxVolumeSetting() const {
    return this->sfxVolume * this->maxSfxVolume;
}

float GestionnaireAudio::getMaxSfxVolume() const {
    return this->maxSfxVolume;
}

void GestionnaireAudio::setMusicVolumeAnimation(float v) {
    float parsedVolume = v;
    if (v >= 1.0f) {
        parsedVolume = 1.0f;
    }
    else if (v <= 0.0f) {
        parsedVolume = 0.0f;
    }
    musicOutput->setVolume(v * this->maxMusicVolume);
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
