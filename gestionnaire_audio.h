#ifndef GESTIONNAIRE_AUDIO_H
#define GESTIONNAIRE_AUDIO_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMap>
#include <QSettings>
#include <QSoundEffect>

enum AudioMode { MUSIQUE, SFX };

class GestionnaireAudio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float musicVolume READ getMusicVolume WRITE setMusicVolumeAnimation)
public:
    GestionnaireAudio(QObject* parent = nullptr);

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
    void setMusicVolumeAnimation(float v);

    void addSfx(const QString& name, const QString& path, int voices);
    void playSfx(const QString& name);
    void setSfxVolume(float v);
    float getSfxVolume() const;
    float getSfxVolumeSetting() const;
    float getMaxSfxVolume() const;

    void sauvegarderParametres() const;
    void chargerParametres();

private slots:
    void onMediaFinished(QMediaPlayer::MediaStatus status);

private:
    const float maxMusicVolume = 0.025f;
    const float maxSfxVolume = 0.025f;

    float musicVolume = 0.0f;
    float sfxVolume = 0.0f;

    QMediaPlayer* musicPlayer;
    QAudioOutput* musicOutput;

    QStringList playlist;
    int index = 0;

    struct SfxPool {
        QVector<QSoundEffect*> players;
        int nextIndex = 0;
    };

    QMap<QString, SfxPool> sfx;
};

#endif