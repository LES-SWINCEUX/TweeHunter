#include "panneau_scores.h"

PanneauScores::PanneauScores(QWidget* parent)
    : PanneauMenu(parent)
{
    int id = QFontDatabase::addApplicationFont(QDir::currentPath() + "/fonts/pixel.ttf");

    QString famille = (id >= 0)
        ? QFontDatabase::applicationFontFamilies(id).at(0)
        : "Courier New";

    fontPixel.setFamily(famille);
    fontPixel.setBold(true);
    fontPixel.setLetterSpacing(QFont::AbsoluteSpacing, 1);

    initialiserPanneau();
}

void PanneauScores::creer()
{
    imgPanneau = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/panneau_scores.png");

    labelTitre = new QLabel("", this);
    labelTitre->hide();

    const int NB_LIGNES_AFF = std::min(5, GestionnaireScores::MAX_SCORES);

    for (int i = 0; i < NB_LIGNES_AFF; ++i) {
        Ligne l;
        l.rang = setupTexteMenu(QString::number(i + 1) + ".");
        l.nom = setupTexteMenu("-");
        l.score = setupTexteMenu("-");
        lignes.append(l);
    }

    boutonRetour = new Bouton("/images/menu/retour_spritesheet.png", 3, this);
    connect(boutonRetour, &Bouton::clicked, this, &PanneauMenu::demanderRetourOptions);

    rafraichirLignes();
}

void PanneauScores::rafraichirLignes()
{
    const QList<EntreeScore> & scores = GestionnaireScores::instance().scores();

    for (int i = 0; i < lignes.size(); ++i) {
        if (i < scores.size()) {
            lignes[i].nom->setTexte(scores[i].nom);
            lignes[i].score->setTexte(QString::number(scores[i].score));
        }
        else {
            lignes[i].nom->setTexte("-");
            lignes[i].score->setTexte("-");
        }
    }
}

void PanneauScores::positionner() {
    if (lignes.isEmpty()) {
        return;
    }

    const int W = width();
    const int H = height();

    if (W <= 0 || H <= 0) {
        return;
    }

    const int panW = std::max(400, int(W * 0.52f));
    const int panH = std::max(300, int(panW * 0.72f));

    const int panX = (W - panW) / 2;
    const int panY = (H - panH) / 2;

    const int contentTop = panY + int(panH * 0.36f);
    const int contentBot = panY + int(panH * 0.80f);
    const int contentH = std::max(1, contentBot - contentTop);
    const int contentX = panX + int(panW * 0.10f);
    const int contentW = panW - int(panW * 0.17f);

    if (boutonRetour) {
        int btnH = std::max(18, int(panH * 0.10f));
        QSize ti = boutonRetour->tailleImage();
        if (ti.height() > 0) {
            float ech = float(btnH) / float(ti.height());
            ech = std::clamp(ech, 0.2f, 1.5f);
            boutonRetour->setEchelle(ech);
        }

        const int marginBottom = std::max(6, int(panH * 0.05f));
        int bx = panX + (panW - boutonRetour->width()) / 2;
        int by = panY + panH - marginBottom - boutonRetour->height();

        boutonRetour->move(bx, by);
        boutonRetour->raise();
        boutonRetour->show();
    }

    const int colRangW = int(contentW * 0.10f);
    const int pad = int(contentW * 0.02f);

    const int n = lignes.size();
    const float bandH = float(contentH) / float(n);
    int ligneHReelle = std::max(22, int(bandH * 0.55f));
    int fontSize = std::max(14, int(ligneHReelle * 0.92f));

    QFont f = fontPixel;
    f.setPixelSize(fontSize);
    f.setBold(true);

    QFontMetrics fm(f);

    const QString worstCase = "999999";
    const int contourMargin = 6;
    int colScoreW = fm.horizontalAdvance(worstCase) + contourMargin * 2;

    colScoreW = std::clamp(colScoreW, int(contentW * 0.14f), int(contentW * 0.28f));

    const int scoreInset = int(panW * 0.03f);

    const int xRang = contentX;
    const int xScore = contentX + contentW - colScoreW - scoreInset;

    int centreX = panX + panW / 2;
    if (boutonRetour) {
        centreX = boutonRetour->geometry().center().x();
    }

    const int nomMinX = xRang + colRangW + pad;
    const int nomMaxX = xScore - pad;
    const int nomMaxW = std::max(0, nomMaxX - nomMinX);

    const int nomW = std::min(nomMaxW,
        2 * std::min(centreX - nomMinX,
            nomMaxX - centreX));

    const int colNomW = std::max(0, nomW);
    const int xNom = centreX - colNomW / 2;

    const float gapOffsetInBand = 0.14f;

    for (int i = 0; i < n; ++i)
    {
        int y = int(contentTop + i * bandH + bandH * gapOffsetInBand);

        lignes[i].rang->setFont(f);
        lignes[i].nom->setFont(f);
        lignes[i].score->setFont(f);

        lignes[i].rang->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lignes[i].nom->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lignes[i].score->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        if (i == 0) {
            lignes[i].score->setRemplissage(QColor("#FFD700"));
        }

        if (i == 1) {
            lignes[i].score->setRemplissage(QColor("#C0C0C0"));
        }

        if (i == 2) {
            lignes[i].score->setRemplissage(QColor("#CD7F32"));
        }

        lignes[i].rang->setGeometry(xRang, y, colRangW, ligneHReelle);
        lignes[i].nom->setGeometry(xNom, y, colNomW, ligneHReelle);
        lignes[i].score->setGeometry(xScore, y, colScoreW, ligneHReelle);

        lignes[i].rang->show();
        lignes[i].nom->show();
        lignes[i].score->show();
    }
}

void PanneauScores::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    if (imgPanneau.isNull()) {
        return;
    }

    QPainter p(this);

    const int W = width();
    const int H = height();

    const int panW = std::max(400, int(W * 0.52f));
    const int panH = std::max(300, int(panW * 0.72f));

    const int panX = (W - panW) / 2;
    const int panY = (H - panH) / 2;

    if (imgPanneau && !imgPanneau->isNull()) {
        p.drawPixmap(panX, panY, panW, panH, *imgPanneau);
    }
}

TexteMenu* PanneauScores::setupTexteMenu(const QString& txt) {
    TexteMenu* w = new TexteMenu(this);
    w->setTexte(txt);

    QFont f = fontPixel;
    f.setBold(true);
    w->setFont(f);

    w->setRemplissage(QColor("#FFE066"));
    w->setContour(QColor("#B11B1B"));
    w->setOmbrage(QColor(0, 0, 0, 200));
    w->setLargeurContour(4.0f);
    w->setOmbrageOffset(QPointF(3.0, 3.0));

    return w;
}