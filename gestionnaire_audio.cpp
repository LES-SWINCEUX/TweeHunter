#include "gestionnaire_audio.h"

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
}

// -------- MUSIQUE --------
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

// -------- SFX --------
void GestionnaireAudio::addSfx(QString name, QString path)
{
    QSoundEffect* s = new QSoundEffect(this);
    s->setSource(QUrl(path));
    s->setVolume(sfxVolume);
    sfx[name] = s;
}

void GestionnaireAudio::playSfx(QString name)
{
    if (sfx.contains(name)) {
        sfx[name]->play();
    }
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
    for (QSoundEffect* sound : sfx) {
        sound->setVolume(parsedVolume * this->maxSfxVolume);
    }
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