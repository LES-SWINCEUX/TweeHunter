#include "ecran_fin_partie.h"

EcranFinPartie::EcranFinPartie(GestionnaireAudio* gestionnaireAudio, QWidget* parent, Touches* touchesParam)
    : QWidget(parent)
    , gestionnaireAudio(gestionnaireAudio)
    , touches(touchesParam)
    , bg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/fin_partie/background.png"))
    , bgPanneau(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/fin_partie/panneau.png"))
    , titreImg(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/fin_partie/titre.png"))
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    fontPixel.setFamily("Luckiest Guy");
    fontPixel.setBold(true);
    fontPixel.setLetterSpacing(QFont::AbsoluteSpacing, 2);

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();

    fadeAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeAnim->setDuration(800);
    fadeAnim->setStartValue(255);
    fadeAnim->setEndValue(0);
    connect(fadeAnim, &QPropertyAnimation::finished, this, [this]() {
        overlay->hide();
    });

    labelVotreScore = makeLabel("VOTRE SCORE :", "#FFE066");
    labelScore = makeLabel("0", "#FFE066");
    labelNom = makeLabel("VALIDEZ VOTRE NOM :", "#FFFFFF");

    champNom = new QLineEdit(this);
    champNom->setPlaceholderText("...");
    champNom->setMaxLength(12);
    champNom->setAlignment(Qt::AlignHCenter);
    champNom->setStyleSheet(
        "QLineEdit {"
        "  background-color: rgba(8, 16, 35, 210);"
        "  border: 2px solid #4A8FCC;"
        "  border-radius: 4px;"
        "  color: #FFFFFF;"
        "  selection-background-color: #5AADFF;"
        "}");

    boutonValider = new Bouton(
        "/images/fin_partie/bouton_validation.png", 3, this);

    connect(boutonValider, &Bouton::clicked, this, [this]() {
        QString nom = champNom->text().trimmed();

        overlay->setAlpha(0);
        overlay->show();
        overlay->raise();

        QPropertyAnimation* sortie = new QPropertyAnimation(overlay, "alpha", this);
        sortie->setEasingCurve(QEasingCurve::InOutQuad);
        sortie->setDuration(600);
        sortie->setStartValue(0);
        sortie->setEndValue(255);
        sortie->start(QAbstractAnimation::DeleteWhenStopped);

        GestionnaireAudio* audio = this->gestionnaireAudio;
        if (audio != nullptr) {
            QPropertyAnimation* fadeMusique = new QPropertyAnimation(audio, "musicVolume", this);
            fadeMusique->setDuration(600);
            fadeMusique->setStartValue(audio->getMusicVolumeSetting());
            fadeMusique->setEndValue(0.0f);
            fadeMusique->start(QAbstractAnimation::DeleteWhenStopped);
        }

        connect(sortie, &QPropertyAnimation::finished, this, [this, nom]() {
            emit retourMenuDemande(nom, score);
        });
    });

    placerElements();

    auto valider = [this]() {
        if (transitionEnCours) return;
        transitionEnCours = true;
        timerManette.stop();
        boutonValider->simulerClic();
    };

    connect(&timerManette, &QTimer::timeout, this, [this]() {
        if (touches) {
            touches->verifierConnexion();
            touches->lireNavigation();
        }
    });
    timerManette.setTimerType(Qt::PreciseTimer);
    timerManette.setInterval(16);

    if (touches) {
        connect(touches, &Touches::naviguerConfirmer, this, valider);
    }
}

void EcranFinPartie::setScore(int s)
{
    score = s;
    if (labelScore)
        labelScore->setText(QString::number(score));
}

void EcranFinPartie::setNomParDefaut(const QString& nom)
{
    nomParDefaut = nom.trimmed().toUpper();
    if (champNom && champNom->text().trimmed().isEmpty() && !nomParDefaut.isEmpty()) {
        champNom->setText(nomParDefaut);
    }
}

void EcranFinPartie::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    if (champNom && !nomParDefaut.isEmpty()) {
        champNom->setText(nomParDefaut);
    }
    placerElements();
    lancerFadeIn();

    transitionEnCours = false;

    const bool manetteConnectee = (touches && touches->isJoystickConnected()) || (touches && touches->isJoystickPersoConnected());

    if (manetteConnectee) {
        initialiserManette();
        timerManette.start();
        if (boutonValider) {
            boutonValider->setSelectionneManette(true);
        }
    }
}

void EcranFinPartie::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    bg.mettreAJour(size());
    bgPanneau.mettreAJour(size());

    titreCache = QPixmap();
    titreCacheRect = QRect();
    if (titreImg && !titreImg->isNull()) {
        QRect pan = srcRectToScreen(width(), height(), PAN_SRC_X, PAN_SRC_Y, PAN_SRC_W, PAN_SRC_H);
        int titreH = int(pan.height() * 0.22f);
        titreCache = titreImg->scaledToHeight(titreH, Qt::SmoothTransformation);
        int titreW = titreCache.width();
        float offsetRatio = 0.001f;
        int offsetGauche = int(pan.width() * offsetRatio);
        int titreX = pan.x() + (pan.width() - titreW) / 2 - offsetGauche;
        int titreY = pan.top() - int(titreH * 0.75f);
        titreCacheRect = QRect(titreX, titreY, titreW, titreH);
    }

    if (overlay) {
        overlay->setGeometry(rect());
        overlay->raise();
    }

    placerElements();
}

void EcranFinPartie::placerElements()
{
    const int W = width();
    const int H = height();
    if (W == 0 || H == 0) {
        return;
    }

    panneau = srcRectToScreen(W, H, PAN_SRC_X, PAN_SRC_Y, PAN_SRC_W, PAN_SRC_H);

    const int panX = panneau.x();
    const int panY = panneau.y();
    const int panW = panneau.width();
    const int panH = panneau.height();
    const int padX = int(panW * CHAMP_PAD_X);
    const int zoneW = panW - 2 * padX;

    if (labelVotreScore) {
        QFont f = fontPixel; f.setPixelSize(std::max(8, int(panH * 0.090f)));
        labelVotreScore->setFont(f);
        labelVotreScore->setGeometry(geometrieLabel(LABEL_SCORE_Y, LABEL_SCORE_H, 1.0f, zoneW, panX, panY, panW, panH));
    }
    if (labelScore) {
        QFont f = fontPixel; f.setPixelSize(std::max(8, int(panH * 0.160f)));
        labelScore->setFont(f);
        labelScore->setGeometry(geometrieLabel(SCORE_VAL_Y, SCORE_VAL_H, 1.0f, zoneW, panX, panY, panW, panH));
    }
    if (labelNom) {
        QFont f = fontPixel; f.setPixelSize(std::max(8, int(panH * 0.085f)));
        labelNom->setFont(f);
        labelNom->setGeometry(geometrieLabel(LABEL_NOM_Y, LABEL_NOM_H, 1.0f, zoneW, panX, panY, panW, panH));
    }

    if (champNom) {
        QFont f = fontPixel;
        f.setPixelSize(std::max(8, int(panH * 0.100f)));
        champNom->setFont(f);
        int champW = int(zoneW * 0.7f);
        int champX = panX + (panW - champW) / 2;
        champNom->setGeometry(champX, panY + int(panH * CHAMP_Y), champW, int(panH * CHAMP_H));
    }

    if (boutonValider) {
        int hCible = int(panH * BTN_H_RATIO);
        float ech  = float(hCible) / float(boutonValider->tailleImage().height());
        ech = std::clamp(ech, 0.2f, 2.0f);
        boutonValider->setEchelle(ech);
        int bx = panX + (panW - boutonValider->width()) / 2;
        int by = panY + int(panH * BTN_Y);
        boutonValider->move(bx, by);
    }
}

void EcranFinPartie::lancerFadeIn()
{
    if (!overlay || !fadeAnim) {
        return;
    }

    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();
    fadeAnim->stop();
    fadeAnim->start();
}

QRect EcranFinPartie::srcRectToScreen(int screenW, int screenH, int srcX, int srcY, int srcW, int srcH)
{
    float scaleX = float(screenW) / float(SRC_W);
    float scaleY = float(screenH) / float(SRC_H);
    float scale = std::max(scaleX, scaleY);

    float scaledW = SRC_W * scale;
    float scaledH = SRC_H * scale;

    float cropX = (scaledW - screenW) / 2.0f;
    float cropY = (scaledH - screenH) / 2.0f;

    int x = int(srcX * scale - cropX);
    int y = int(srcY * scale - cropY);
    int w = int(srcW * scale);
    int h = int(srcH * scale);

    return QRect(x, y, w, h);
}

QLabel* EcranFinPartie::makeLabel(const QString& txt, const QString& couleur) {
    QLabel* l = new QLabel(txt, this);
    l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l->setStyleSheet(
        QString("QLabel { color: %1; background: transparent; }").arg(couleur));
    return l;
}

QRect EcranFinPartie::geometrieLabel(float ratioY, float ratioH, float ratioW, int zoneW, int panX, int panY, int panW, int panH) {
    int lw = int(zoneW * ratioW);
    int lx = panX + (panW - lw) / 2;
    int ly = panY + int(panH * ratioY);
    int lh = int(panH * ratioH);
    return QRect(lx, ly, lw, lh);
}

void EcranFinPartie::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    bg.dessiner(painter);
    if (!bg.estValide()) {
        painter.fillRect(rect(), Qt::black);
    }

    bgPanneau.dessiner(painter);

    if (!titreCache.isNull()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(titreCacheRect, titreCache);
    }
}

void EcranFinPartie::initialiserManette()
{
    if (touches) touches->verifierConnexion();
}
