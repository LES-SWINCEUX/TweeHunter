#include "ecran_regles.h"

EcranRegles::EcranRegles(GestionnaireAudio* gestionnaireAudio,
    Touches* touches,
    QWidget* parent)
    : QWidget(parent),
    gestionnaireAudio(gestionnaireAudio),
    touches(touches),
    arrierePlan(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/background.png"))
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);

    creerInterface();
    initialiserCartes();
    connecterSignaux();

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();

    fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeInAnim->setDuration(500);
    fadeInAnim->setStartValue(255);
    fadeInAnim->setEndValue(0);
    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        if (overlay) {
            overlay->hide();
        }
    });

    if (gestionnaireAudio) {
        fadeOutMusique = new QPropertyAnimation(gestionnaireAudio, "musicVolume", this);
        fadeOutMusique->setEasingCurve(QEasingCurve::InOutQuad);
        fadeOutMusique->setDuration(600);
    }

    fadeOutAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeOutAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeOutAnim->setDuration(500);

    timerAnimation = new QTimer(this);
    timerAnimation->setInterval(33);
    connect(timerAnimation, &QTimer::timeout, this, &EcranRegles::mettreAJourAnimation);

    ajusterTailleBoutons();
    mettreAJourFondCache();
    mettreAJourMiseEnPage();
    placerElements();
}

void EcranRegles::setConfiguration(const ConfigurationPartie& config)
{
    configuration = config;
    tempsAnimationMs = 0;
    initialiserCartes();
    ajusterTailleBoutons();
    mettreAJourMiseEnPage();
    placerElements();
    update();
}

void EcranRegles::creerInterface()
{
    boutonCommencer = new Bouton("/images/menu/jouer_spritesheet_horizontal.png", 3, this);
    boutonCommencer->setCursor(Qt::PointingHandCursor);

    boutonRetour = new Bouton("/images/menu/retour_spritesheet.png", 3, this);
    boutonRetour->setCursor(Qt::PointingHandCursor);
}

void EcranRegles::connecterSignaux()
{
    connect(boutonCommencer, &Bouton::clicked, this, &EcranRegles::lancerDemarrage);
    connect(boutonRetour, &Bouton::clicked, this, &EcranRegles::lancerRetour);
}

void EcranRegles::ajusterTailleBoutons()
{
    const int h = qMax(520, height());
    const int hauteurBouton = qMax(30, h / 15);

    auto ajuster = [&](Bouton* bouton)
        {
            if (!bouton) {
                return;
            }

            const QSize tailleBase = bouton->tailleImage();
            if (!tailleBase.isValid() || tailleBase.height() <= 0) {
                return;
            }

            const double echelle = static_cast<double>(hauteurBouton) / static_cast<double>(tailleBase.height());
            bouton->setEchelle(static_cast<float>(echelle));
        };

    ajuster(boutonRetour);
    ajuster(boutonCommencer);
}

QVector<QPixmap> EcranRegles::extraireFramesSpritesheet(const QString& cheminSprite,
    int colonnes,
    int lignes)
{
    QVector<QPixmap> frames;

    auto spriteSheet = SpriteManager::instance().getPixmap(QDir::currentPath() + cheminSprite);

    if (!spriteSheet || spriteSheet->isNull()) {
        return frames;
    }

    if (colonnes <= 0 || lignes <= 0) {
        return frames;
    }

    if (spriteSheet->width() < colonnes || spriteSheet->height() < lignes) {
        return frames;
    }

    const int largeurFrame = spriteSheet->width() / colonnes;
    const int hauteurFrame = spriteSheet->height() / lignes;

    if (largeurFrame <= 0 || hauteurFrame <= 0) {
        return frames;
    }

    for (int ligne = 0; ligne < lignes; ++ligne) {
        for (int colonne = 0; colonne < colonnes; ++colonne) {
            const QRect sourceRect(
                colonne * largeurFrame,
                ligne * hauteurFrame,
                largeurFrame,
                hauteurFrame
            );

            if (sourceRect.right() < spriteSheet->width() &&
                sourceRect.bottom() < spriteSheet->height()) {
                frames.push_back(spriteSheet->copy(sourceRect));
            }
        }
    }

    return frames;
}

QVector<QString> EcranRegles::obtenirCheminsPoisonSelonMode(ModeJeu mode) const
{
    switch (mode) {
    case ModeJeu::PLUS_18:
        return kSpritesPoisonPlus18;

    case ModeJeu::MOINS_18:
    default:
        return kSpritesPoisonMoins18;
    }
}

QVector<QString> EcranRegles::obtenirCheminsGatorSelonMode(ModeJeu mode) const
{
    switch (mode) {
    case ModeJeu::PLUS_18:
        return kSpritesGatorPlus18;

    case ModeJeu::MOINS_18:
    default:
        return kSpritesGatorMoins18;
    }
}

QVector<QString> EcranRegles::obtenirCheminsDebuffSelonMode(ModeJeu mode) const
{
    switch (mode) {
    case ModeJeu::PLUS_18:
        return kSpritesDebuffPlus18;

    case ModeJeu::MOINS_18:
    default:
        return kSpritesDebuffMoins18;
    }
}

QVector<QString> EcranRegles::obtenirCheminsWaterSelonMode(ModeJeu mode) const
{
    switch (mode) {
    case ModeJeu::PLUS_18:
        return kSpritesWaterPlus18;

    case ModeJeu::MOINS_18:
    default:
        return kSpritesWaterMoins18;
    }
}

void EcranRegles::initialiserCartes()
{
    cartesRegles.clear();

    {
        CarteRegle carte;
        carte.titre = "Poison";
        carte.description = "Si tu la tires, tu perds 1 vie.";
        carte.couleurAccent = QColor(210, 70, 70);

        const QVector<QString> cheminsPoison = obtenirCheminsPoisonSelonMode(configuration.modeJeu);
        for (int i = 0; i < cheminsPoison.size(); ++i) {
            QVector<QPixmap> frames = extraireFramesSpritesheet(cheminsPoison[i],
                kColonnesSpritesheet,
                kLignesSpritesheet);
            if (!frames.isEmpty()) {
                carte.animations.push_back(frames);
            }
        }

        carte.indexAnimation = 0;
        carte.indexFrame = 0;
        carte.tempsAccumuleFrameMs = 0;
        carte.dureeFrameMs = 110;
        carte.changerAnimationApresCycle = true;
        carte.phaseOscillation = 0.0f;
        carte.amplitudeOscillation = 9;
        cartesRegles.push_back(carte);
    }

    {
        CarteRegle carte;
        carte.titre = "Gatorade";
        carte.description = "Si tu la tires, tu gagnes une demi-vie.";
        carte.couleurAccent = QColor(70, 185, 100);

        const QVector<QString> cheminsGator = obtenirCheminsGatorSelonMode(configuration.modeJeu);
        for (int i = 0; i < cheminsGator.size(); ++i) {
            QVector<QPixmap> frames = extraireFramesSpritesheet(cheminsGator[i],
                kColonnesSpritesheet,
                kLignesSpritesheet);
            if (!frames.isEmpty()) {
                carte.animations.push_back(frames);
            }
        }

        carte.indexAnimation = 0;
        carte.indexFrame = 4;
        carte.tempsAccumuleFrameMs = 0;
        carte.dureeFrameMs = 110;
        carte.changerAnimationApresCycle = true;
        carte.phaseOscillation = 1.4f;
        carte.amplitudeOscillation = 11;
        cartesRegles.push_back(carte);
    }

    {
        CarteRegle carte;
        carte.titre = "Splash";
        carte.description = "Si tu la tires, elle ajoute un splash.";
        carte.couleurAccent = QColor(70, 150, 230);

        const QVector<QString> cheminsDebuff = obtenirCheminsDebuffSelonMode(configuration.modeJeu);
        for (int i = 0; i < cheminsDebuff.size(); ++i) {
            QVector<QPixmap> frames = extraireFramesSpritesheet(cheminsDebuff[i],
                kColonnesSpritesheet,
                kLignesSpritesheet);
            if (!frames.isEmpty()) {
                carte.animations.push_back(frames);
            }
        }

        carte.indexAnimation = 0;
        carte.indexFrame = 0;
        carte.tempsAccumuleFrameMs = 0;
        carte.dureeFrameMs = 110;
        carte.changerAnimationApresCycle = true;
        carte.phaseOscillation = 2.5f;
        carte.amplitudeOscillation = 10;
        cartesRegles.push_back(carte);
    }

    {
        CarteRegle carte;
        carte.titre = "Eau";
        carte.description = "Si tu la manques, tu perds 1 vie.";
        carte.couleurAccent = QColor(80, 190, 255);

        const QVector<QString> cheminsWater = obtenirCheminsWaterSelonMode(configuration.modeJeu);
        for (int i = 0; i < cheminsWater.size(); ++i) {
            QVector<QPixmap> frames = extraireFramesSpritesheet(cheminsWater[i],
                kColonnesSpritesheet,
                kLignesSpritesheet);
            if (!frames.isEmpty()) {
                carte.animations.push_back(frames);
            }
        }

        carte.indexAnimation = 0;
        carte.indexFrame = 2;
        carte.tempsAccumuleFrameMs = 0;
        carte.dureeFrameMs = 110;
        carte.changerAnimationApresCycle = true;
        carte.phaseOscillation = 3.3f;
        carte.amplitudeOscillation = 8;
        cartesRegles.push_back(carte);
    }
}

QFont EcranRegles::creerPoliceCompteur(int pixelSize) const
{
    QFont font("Press Start 2P");
    font.setPixelSize(qMax(1, pixelSize));
    font.setStyleStrategy(QFont::NoAntialias);
    return font;
}

QFont EcranRegles::creerPoliceTexte(int pixelSize, bool gras) const
{
    QFont font;
    font.setPixelSize(qMax(1, pixelSize));
    font.setBold(gras);
    return font;
}

int EcranRegles::calculerHauteurCarte(const CarteRegle& carte, int largeurCarte) const
{
    const int marge = qMax(10, largeurCarte / 18);
    const int interligne = qMax(4, largeurCarte / 50);
    const int largeurTexte = qMax(40, largeurCarte - 2 * marge);

    const int tailleTitre = qMax(9, qMin(13, largeurCarte / 18));
    const int tailleDescription = qMax(11, qMin(15, largeurCarte / 20));

    QFont policeTitre = creerPoliceCompteur(tailleTitre);
    QFontMetrics metricsTitre(policeTitre);

    QRect rectTitre = metricsTitre.boundingRect(
        QRect(0, 0, largeurTexte, 1000),
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.titre
    );

    QFont policeDescription = creerPoliceTexte(tailleDescription, false);
    QFontMetrics metricsDescription(policeDescription);

    QRect rectDescription = metricsDescription.boundingRect(
        QRect(0, 0, largeurTexte, 1000),
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.description
    );

    const int hauteurImage = qMax(56, qMin(110, largeurCarte / 2));

    return marge
        + hauteurImage
        + interligne
        + rectTitre.height()
        + interligne
        + rectDescription.height()
        + marge;
}

void EcranRegles::mettreAJourMiseEnPage()
{
    miseEnPage.cartes.clear();
    ajusterTailleBoutons();

    const int w = width();
    const int h = height();

    const int margeExterieureX = qMax(24, w / 22);
    const int margeExterieureY = qMax(20, h / 24);

    const int paddingX = qMax(18, w / 70);
    const int paddingY = qMax(18, h / 45);

    const int ecartTitreSeparateur = 10;
    const int ecartSeparateurCartes = 18;
    const int ecartCartesBoutons = qMax(18, h / 40);
    const int ecartBoutons = qMax(14, w / 55);

    int colonnes = 4;
    if (w < 1180) {
        colonnes = 2;
    }

    if (cartesRegles.size() < colonnes) {
        colonnes = cartesRegles.size();
    }

    const int lignes = (cartesRegles.size() + colonnes - 1) / colonnes;

    const int espaceHorizontalCartes = qMax(12, w / 70);
    const int espaceVerticalCartes = qMax(12, h / 40);

    const int largeurCartePreferee = (colonnes == 4) ? 235 : 300;

    const int largeurBoutons = boutonRetour->width() + ecartBoutons + boutonCommencer->width();

    QFont policeTitre = creerPoliceCompteur(qMax(20, qMin(34, w / 32)));
    QFontMetrics metricsTitre(policeTitre);
    const int largeurTitreMin = metricsTitre.horizontalAdvance("Canettes Spéciales") + 40;

    const int largeurMaxPanneau = w - 2 * margeExterieureX;
    const int largeurInterieureMax = largeurMaxPanneau - 2 * paddingX;

    int largeurCarte = (largeurInterieureMax - (colonnes - 1) * espaceHorizontalCartes) / qMax(1, colonnes);
    largeurCarte = qMin(largeurCarte, largeurCartePreferee);
    largeurCarte = qMax(170, largeurCarte);

    int hauteurCarte = 0;
    for (int i = 0; i < cartesRegles.size(); ++i) {
        hauteurCarte = qMax(hauteurCarte, calculerHauteurCarte(cartesRegles[i], largeurCarte));
    }
    hauteurCarte = qMax(150, hauteurCarte);

    const int largeurGrille = colonnes * largeurCarte + (colonnes - 1) * espaceHorizontalCartes;
    const int hauteurGrille = lignes * hauteurCarte + (lignes - 1) * espaceVerticalCartes;

    const int largeurInterieure = qMax(qMax(largeurGrille, largeurTitreMin), largeurBoutons);
    const int largeurPanneau = qMin(largeurInterieure + 2 * paddingX, largeurMaxPanneau);

    const int hauteurTitre = qMax(26, h / 15);
    const int hauteurBoutons = qMax(boutonRetour->height(), boutonCommencer->height());

    int hauteurPanneau = paddingY
        + hauteurTitre
        + ecartTitreSeparateur
        + 1
        + ecartSeparateurCartes
        + hauteurGrille
        + ecartCartesBoutons
        + hauteurBoutons
        + paddingY;

    const int hauteurMaxPanneau = h - 2 * margeExterieureY;
    if (hauteurPanneau > hauteurMaxPanneau) {
        const int hauteurFixeSansGrille = paddingY
            + hauteurTitre
            + ecartTitreSeparateur
            + 1
            + ecartSeparateurCartes
            + ecartCartesBoutons
            + hauteurBoutons
            + paddingY;

        int hauteurDisponiblePourGrille = hauteurMaxPanneau - hauteurFixeSansGrille;
        hauteurDisponiblePourGrille -= (lignes - 1) * espaceVerticalCartes;

        if (hauteurDisponiblePourGrille > 0) {
            int nouvelleHauteurCarte = hauteurDisponiblePourGrille / qMax(1, lignes);
            hauteurCarte = qMax(120, qMin(hauteurCarte, nouvelleHauteurCarte));
        }

        hauteurPanneau = paddingY
            + hauteurTitre
            + ecartTitreSeparateur
            + 1
            + ecartSeparateurCartes
            + lignes * hauteurCarte
            + (lignes - 1) * espaceVerticalCartes
            + ecartCartesBoutons
            + hauteurBoutons
            + paddingY;
    }

    const int xPanneau = (w - largeurPanneau) / 2;
    int yPanneau = (h - hauteurPanneau) / 2;
    yPanneau = qMax(margeExterieureY, yPanneau);

    miseEnPage.panneau = QRect(
        xPanneau,
        yPanneau,
        largeurPanneau,
        hauteurPanneau
    );

    miseEnPage.titre = QRect(
        miseEnPage.panneau.left() + paddingX,
        miseEnPage.panneau.top() + paddingY,
        miseEnPage.panneau.width() - 2 * paddingX,
        hauteurTitre
    );

    const int ySeparateur = miseEnPage.titre.bottom() + ecartTitreSeparateur;
    miseEnPage.separateur = QRect(
        miseEnPage.panneau.left() + paddingX,
        ySeparateur,
        miseEnPage.panneau.width() - 2 * paddingX,
        1
    );

    const int largeurGrilleReelle = colonnes * largeurCarte + (colonnes - 1) * espaceHorizontalCartes;
    const int xGrille = miseEnPage.panneau.left() + (miseEnPage.panneau.width() - largeurGrilleReelle) / 2;
    const int yGrille = miseEnPage.separateur.bottom() + ecartSeparateurCartes;

    for (int i = 0; i < cartesRegles.size(); ++i) {
        const int ligne = i / colonnes;
        const int colonne = i % colonnes;

        const QRect rectCarte(
            xGrille + colonne * (largeurCarte + espaceHorizontalCartes),
            yGrille + ligne * (hauteurCarte + espaceVerticalCartes),
            largeurCarte,
            hauteurCarte
        );

        miseEnPage.cartes.push_back(rectCarte);
    }
}

void EcranRegles::placerElements()
{
    if (!boutonRetour || !boutonCommencer) {
        return;
    }

    const int ecartBoutons = qMax(14, width() / 55);
    const int largeurTotale = boutonRetour->width() + ecartBoutons + boutonCommencer->width();
    const int xDepart = miseEnPage.panneau.left() + (miseEnPage.panneau.width() - largeurTotale) / 2;

    const int paddingBas = qMax(18, height() / 45);
    const int hauteurBoutons = qMax(boutonRetour->height(), boutonCommencer->height());
    const int yBoutons = miseEnPage.panneau.bottom() - paddingBas - hauteurBoutons;

    boutonRetour->move(
        xDepart,
        yBoutons + (hauteurBoutons - boutonRetour->height()) / 2
    );

    boutonCommencer->move(
        xDepart + boutonRetour->width() + ecartBoutons,
        yBoutons + (hauteurBoutons - boutonCommencer->height()) / 2
    );
}

void EcranRegles::demarrerAnimation()
{
    if (timerAnimation && !timerAnimation->isActive()) {
        timerAnimation->start();
    }
}

void EcranRegles::arreterAnimation()
{
    if (timerAnimation && timerAnimation->isActive()) {
        timerAnimation->stop();
    }
}

void EcranRegles::avancerAnimationCarte(CarteRegle& carte, int deltaMs)
{
    if (carte.animations.isEmpty()) {
        return;
    }

    if (carte.indexAnimation < 0 || carte.indexAnimation >= carte.animations.size()) {
        carte.indexAnimation = 0;
    }

    if (carte.animations[carte.indexAnimation].isEmpty()) {
        return;
    }

    carte.tempsAccumuleFrameMs += deltaMs;

    while (carte.tempsAccumuleFrameMs >= carte.dureeFrameMs) {
        carte.tempsAccumuleFrameMs -= carte.dureeFrameMs;
        carte.indexFrame++;

        const int nbFramesAnimationCourante = carte.animations[carte.indexAnimation].size();

        if (carte.indexFrame >= nbFramesAnimationCourante) {
            carte.indexFrame = 0;

            if (carte.changerAnimationApresCycle && carte.animations.size() > 1) {
                carte.indexAnimation = (carte.indexAnimation + 1) % carte.animations.size();

                if (carte.animations[carte.indexAnimation].isEmpty()) {
                    carte.indexAnimation = 0;
                }
            }
        }
    }
}

void EcranRegles::mettreAJourAnimation()
{
    if (!timerAnimation) {
        return;
    }

    const int deltaMs = timerAnimation->interval();
    tempsAnimationMs += deltaMs;

    for (int i = 0; i < cartesRegles.size(); ++i) {
        avancerAnimationCarte(cartesRegles[i], deltaMs);
    }

    update();
}

int EcranRegles::bornerIndex(int valeur, int minimum, int maximum) const
{
    if (valeur < minimum) {
        return minimum;
    }

    if (valeur > maximum) {
        return maximum;
    }

    return valeur;
}

const QPixmap* EcranRegles::obtenirFrameActuelle(const CarteRegle& carte) const
{
    if (carte.animations.isEmpty()) {
        return nullptr;
    }

    if (carte.indexAnimation < 0 || carte.indexAnimation >= carte.animations.size()) {
        return nullptr;
    }

    const QVector<QPixmap>& animationCourante = carte.animations[carte.indexAnimation];
    if (animationCourante.isEmpty()) {
        return nullptr;
    }

    const int indexFrameValide = bornerIndex(carte.indexFrame, 0, animationCourante.size() - 1);
    return &animationCourante[indexFrameValide];
}

int EcranRegles::calculerOffsetVertical(const CarteRegle& carte) const
{
    const double tempsSecondes = static_cast<double>(tempsAnimationMs) / 1000.0;
    const double vitesse = 2.2;
    const double valeur = std::sin((tempsSecondes * vitesse * 2.0 * 3.14159265358979323846) + carte.phaseOscillation);

    return static_cast<int>(std::round(valeur * static_cast<double>(carte.amplitudeOscillation)));
}

void EcranRegles::mettreAJourFondCache()
{
    if (!arrierePlan || arrierePlan->isNull() || size().isEmpty()) {
        arrierePlanCache = QPixmap();
        return;
    }

    arrierePlanCache = arrierePlan->scaled(
        size(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );
}

void EcranRegles::dessinerArrierePlan(QPainter& painter)
{
    if (!arrierePlanCache.isNull()) {
        const int x = (arrierePlanCache.width() - width()) / 2;
        const int y = (arrierePlanCache.height() - height()) / 2;
        painter.drawPixmap(0, 0, arrierePlanCache, x, y, width(), height());
    }
    else {
        painter.fillRect(rect(), QColor(20, 20, 40));
    }
}

void EcranRegles::dessinerPanneau(QPainter& painter)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 160));
    painter.drawRoundedRect(miseEnPage.panneau, 18, 18);
}

void EcranRegles::dessinerTitre(QPainter& painter)
{
    QFont police = creerPoliceCompteur(qMax(20, qMin(34, width() / 32)));

    painter.setFont(police);
    painter.setPen(QColor(255, 220, 50));
    painter.drawText(miseEnPage.titre, Qt::AlignHCenter | Qt::AlignVCenter, "Canettes Spéciales");
}

void EcranRegles::dessinerSeparateur(QPainter& painter)
{
    painter.setPen(QPen(QColor(255, 220, 50, 120), 1));
    painter.drawLine(
        miseEnPage.separateur.left(),
        miseEnPage.separateur.center().y(),
        miseEnPage.separateur.right(),
        miseEnPage.separateur.center().y()
    );
}

void EcranRegles::dessinerCartes(QPainter& painter)
{
    const int nombre = qMin(miseEnPage.cartes.size(), cartesRegles.size());

    for (int i = 0; i < nombre; ++i) {
        dessinerCarte(painter, miseEnPage.cartes[i], cartesRegles[i]);
    }
}

void EcranRegles::dessinerCarte(QPainter& painter, const QRect& rect, const CarteRegle& carte)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(25, 25, 25, 190));
    painter.drawRoundedRect(rect, 18, 18);

    painter.setPen(QPen(carte.couleurAccent, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), 18, 18);

    const int marge = qMax(10, rect.width() / 18);
    const int interligne = qMax(4, rect.width() / 50);

    const int tailleTitre = qMax(9, qMin(13, rect.width() / 18));
    const int tailleDescription = qMax(11, qMin(15, rect.width() / 20));

    QFont policeTitre = creerPoliceCompteur(tailleTitre);
    QFontMetrics metricsTitre(policeTitre);

    QFont policeDescription = creerPoliceTexte(tailleDescription, false);
    QFontMetrics metricsDescription(policeDescription);

    const int largeurTexte = qMax(40, rect.width() - 2 * marge);

    QRect rectMesureTitre = metricsTitre.boundingRect(
        QRect(0, 0, largeurTexte, 1000),
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.titre
    );

    QRect rectMesureDescription = metricsDescription.boundingRect(
        QRect(0, 0, largeurTexte, 1000),
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.description
    );

    const int hauteurTitre = rectMesureTitre.height();
    const int hauteurDescription = rectMesureDescription.height();

    int hauteurImage = rect.height() - 2 * marge - hauteurTitre - hauteurDescription - 2 * interligne;
    hauteurImage = qMax(40, qMin(110, hauteurImage));

    QRect zoneImage(
        rect.left() + marge,
        rect.top() + marge,
        rect.width() - 2 * marge,
        hauteurImage
    );

    const QPixmap* frame = obtenirFrameActuelle(carte);
    if (frame && !frame->isNull()) {
        const QPixmap scaled = frame->scaled(
            zoneImage.size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );

        const int offsetY = calculerOffsetVertical(carte);
        const int x = zoneImage.center().x() - scaled.width() / 2;
        const int y = zoneImage.center().y() - scaled.height() / 2 + offsetY;

        painter.drawPixmap(x, y, scaled);
    }

    painter.setFont(policeTitre);
    painter.setPen(Qt::white);

    QRect rectTitre(
        rect.left() + marge + 2,
        zoneImage.bottom() + interligne,
        rect.width() - 2 * marge - 4,
        hauteurTitre
    );

    painter.drawText(
        rectTitre,
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.titre
    );

    painter.setFont(policeDescription);
    painter.setPen(QColor(230, 230, 230));

    const int topDescription = rectTitre.bottom() + interligne;
    const int hauteurDescriptionDisponible = qMax(0, rect.bottom() - marge - topDescription);

    QRect rectDescription(
        rect.left() + marge,
        topDescription,
        rect.width() - 2 * marge,
        hauteurDescriptionDisponible
    );

    painter.drawText(
        rectDescription,
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        carte.description
    );
}

void EcranRegles::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    dessinerArrierePlan(painter);
    dessinerPanneau(painter);
    dessinerTitre(painter);
    dessinerSeparateur(painter);
    dessinerCartes(painter);
}

void EcranRegles::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    mettreAJourFondCache();
    mettreAJourMiseEnPage();
    placerElements();

    if (overlay) {
        overlay->setGeometry(rect());
    }
}

void EcranRegles::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    transitionEnCours = false;
    setEnabled(true);

    mettreAJourFondCache();
    mettreAJourMiseEnPage();
    placerElements();
    demarrerAnimation();

    if (overlay) {
        overlay->setGeometry(rect());
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();
    }

    if (fadeInAnim) {
        fadeInAnim->stop();
        fadeInAnim->start();
    }
}

void EcranRegles::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    arreterAnimation();
}

void EcranRegles::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space) {
        lancerDemarrage();
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        lancerRetour();
        return;
    }

    QWidget::keyPressEvent(event);
}

void EcranRegles::lancerTransition(const std::function<void()>& actionFinale, bool fadeMusique)
{
    if (transitionEnCours) {
        return;
    }

    transitionEnCours = true;
    setEnabled(false);

    if (overlay) {
        overlay->setAlpha(0);
        overlay->show();
        overlay->raise();
    }

    if (!fadeOutAnim) {
        if (actionFinale) {
            actionFinale();
        }
        return;
    }

    fadeOutAnim->stop();
    fadeOutAnim->setStartValue(0);
    fadeOutAnim->setEndValue(255);

    QObject::disconnect(fadeOutAnim, nullptr, this, nullptr);
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this, actionFinale]() {
        if (actionFinale) {
            actionFinale();
        }
    });

    if (fadeOutMusique && gestionnaireAudio && fadeMusique) {
        fadeOutMusique->stop();
        fadeOutMusique->setStartValue(gestionnaireAudio->getMusicVolume());
        fadeOutMusique->setEndValue(0.0f);
        fadeOutMusique->start();
    }

    fadeOutAnim->start();
}

void EcranRegles::lancerDemarrage()
{
    lancerTransition([this]() {
        emit demarrerPartieDemande(configuration);
    }, true);
}

void EcranRegles::lancerRetour()
{
    lancerTransition([this]() {
        emit retourParametresDemande();
    }, false);
}