#ifndef GESTIONNAIRE_SCORES_H
#define GESTIONNAIRE_SCORES_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>
#include <QSettings>
#include <QDebug>
#include <algorithm>

struct EntreeScore {
    QString nom;
    int score;
};

class GestionnaireScores : public QObject
{
    Q_OBJECT

public:
    static GestionnaireScores& instance() {
        static GestionnaireScores inst;
        return inst;
    }

    void ajouterScore(const QString& nom, int score);

    void resetScores();

    const QList<EntreeScore>& scores() const { return listeScores; }

    static const int MAX_SCORES = 10;

private:
    GestionnaireScores() { charger(); }
    ~GestionnaireScores() = default;
    GestionnaireScores(const GestionnaireScores&) = delete;
    GestionnaireScores& operator=(const GestionnaireScores&) = delete;

    void charger();

    void sauvegarder();

    QList<EntreeScore> listeScores;
};

#endif
