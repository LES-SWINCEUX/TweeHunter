#include "gestionnaire_scores.h"

void GestionnaireScores::ajouterScore(const QString& nom, int score) {
    if (nom.trimmed().isEmpty()) return;

    EntreeScore e;
    e.nom = nom.trimmed();
    e.score = score;
    listeScores.append(e);

    std::stable_sort(listeScores.begin(), listeScores.end(),
        [](const EntreeScore& a, const EntreeScore& b) { return a.score > b.score; });

    if (listeScores.size() > MAX_SCORES)
        listeScores.resize(MAX_SCORES);

    sauvegarder();
}

void GestionnaireScores::resetScores() {
    QSettings s("TweeHunter", "Scores");
    s.beginGroup("scores");
    s.remove("");
    s.endGroup();
}

void GestionnaireScores::charger() {
    QSettings s("TweeHunter", "Scores");
    qDebug() << "Fichier de scores: " << s.fileName();
    int n = s.beginReadArray("scores");
    for (int i = 0; i < n; ++i) {
        s.setArrayIndex(i);
        EntreeScore e;
        e.nom = s.value("nom").toString();
        e.score = s.value("score").toInt();
        if (!e.nom.isEmpty())
            listeScores.append(e);
    }
    s.endArray();
}

void GestionnaireScores::sauvegarder() {
    QSettings s("TweeHunter", "Scores");
    s.beginWriteArray("scores");
    for (int i = 0; i < listeScores.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue("nom", listeScores[i].nom);
        s.setValue("score", listeScores[i].score);
    }
    s.endArray();
}