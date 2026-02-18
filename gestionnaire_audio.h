#ifndef GESTIONNAIRE_AUDIO_H
#define GESTIONNAIRE_AUDIO_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QMap>

enum AudioMode { MUSIQUE, SFX };

class GestionnaireAudio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float musicVolume READ getMusicVolume WRITE setMusicVolumeAnimation)
public:
    GestionnaireAudio(QObject* parent = nullptr);

    // MUSIQUE
    void setPlaylist(const QStringList& musics);
    void playMusic();
    void nextMusic();
    void setMusicVolume(float v);
    float getMusicVolume() const;
    float getMusicVolumeSetting() const;
    float getMaxMusicVolume() const;
    void stopMusic();
    void clearPlaylist();
    void stopAndClearMusic();

    // SFX
    void addSFX(QString name, QString path);
    void playSFX(QString name);
    void setSFXVolume(float v);
    float getSFXVolume() const;
    float getSFXVolumeSetting() const;
    float getMaxSFXVolume() const;

private slots:
    void onMediaFinished(QMediaPlayer::MediaStatus status);

private:
    const float maxMusicVolume = 0.025f;
    const float maxSFXVolume = 0.025f;

    float musicVolume = 0.0f;
    float sfxVolume = 0.0f;

    QMediaPlayer* musicPlayer;
    QAudioOutput* musicOutput;

    QStringList playlist;
    int index = 0;

    QMap<QString, QSoundEffect*> sfx;

    void setMusicVolumeAnimation(float v);
};

#endif