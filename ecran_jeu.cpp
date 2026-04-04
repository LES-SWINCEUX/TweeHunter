#include "ecran_jeu.h"
#include <iostream>
using namespace std;

EcranJeu::EcranJeu(GestionnaireAudio* gestionnaireAudio, const ConfigurationPartie& configuration, QWidget* parent, Touches* touches)
    : QWidget(parent)
    , configurationPartie(configuration)
    , gestionnaireAudio(gestionnaireAudio)
    , touches(touches)
{
    initReticuleEtArmes();
    initHUD();
    initAudio();
    initAnimations();
    initMinuterie();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

EcranJeu::~EcranJeu()
{
    delete jeu;
    delete armes;
}

void EcranJeu::initReticuleEtArmes()
{
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);

    QPoint pos = mapFromGlobal(QCursor::pos());
    reticule = new Reticule(this, pos, configurationPartie.arme, configurationPartie.manette, touches);
    reticule->show();

    armes = new Armes(configurationPartie.arme, configurationPartie.powerUp);
    armes->setFenetre(this);
    maxBalles = armes->nbMunitions();
    power_up  = armes->nbPowerUp();

    gamepad = reticule->getGamepad();

    if (SDL_Init(SDL_INIT_GAMEPAD) < 0) {
        qDebug() << "Erreur SDL:" << SDL_GetError();
    }

    gestionnaireEntrees = new GestionnaireEntrees(configurationPartie, reticule, gamepad, this);

    connect(gestionnaireEntrees, &GestionnaireEntrees::tireDemande, this, &EcranJeu::tire);
    connect(gestionnaireEntrees, &GestionnaireEntrees::reloadDemande, this, &EcranJeu::rechargerArme);
    connect(gestionnaireEntrees, &GestionnaireEntrees::pauseDemande, this, &EcranJeu::mettreEnPause);
    connect(gestionnaireEntrees, &GestionnaireEntrees::powerUpDemande, this, &EcranJeu::Power);
    connect(gestionnaireEntrees, &GestionnaireEntrees::joystickDeplace, this, [this](float delta) {
        reticule->applyJoystickPerso(this, delta);
    });
}

void EcranJeu::initHUD()
{
    compteurBalles = new CompteurBalles(this, armes->nbMunitions());
    vies = new Vies(this);
    compteurPoints = new CompteurPoints(this);
    compteurPowerUp = new CompteurPowerUp(this, configurationPartie.powerUp, armes->nbPowerUp());

    compteurBalles->setBalles(armes->nbMunitions());
    compteurBalles->show();

    vies->setVies(3);
    vies->show();

    compteurPoints->setNombresNumeros(6);
    compteurPoints->setPoints(0);
    compteurPoints->setAnimation(true);
    compteurPoints->setVitesseAnimation(2, 1);
    compteurPoints->show();

    compteurPowerUp->show();

    if (touches) {
        connect(compteurBalles, &CompteurBalles::ballesChanged, touches, &Touches::envoyerNbBalles);
    }
}

void EcranJeu::initAudio()
{
    if (!gestionnaireAudio) return;

    gestionnaireAudio->stopAndClearMusic();
    gestionnaireAudio->setPlaylist({ QDir::currentPath() + "/sounds/jeu/track_1.mp3" });
    gestionnaireAudio->playMusic();

    estompeMusique = new QPropertyAnimation(gestionnaireAudio, "musicVolume", this);
    estompeMusique->setDuration(1000);
    estompeMusique->setStartValue(0.0);
    estompeMusique->setEndValue(gestionnaireAudio->getMusicVolumeSetting());
    estompeMusique->start(QAbstractAnimation::DeleteWhenStopped);

    gestionnaireAudio->addSfx("gunshot", QDir::currentPath() + "/sounds/sfx/gunshot.wav",        maxBalles);
    gestionnaireAudio->addSfx("gunshot_target", QDir::currentPath() + "/sounds/sfx/gunshot_target.wav", maxBalles);
    gestionnaireAudio->addSfx("gun_empty", QDir::currentPath() + "/sounds/sfx/gun_empty.wav",      maxBalles);
    gestionnaireAudio->addSfx("reload", QDir::currentPath() + "/sounds/sfx/reload.wav",         maxBalles);
}

void EcranJeu::initAnimations()
{
    arrierePlan = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/background.png");
    setAttribute(Qt::WA_OpaquePaintEvent);

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(255);
    overlay->show();
    overlay->raise();

    fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeInAnim->setDuration(1000);
    fadeInAnim->setStartValue(255);
    fadeInAnim->setEndValue(0);
    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() { overlay->hide(); });
}

void EcranJeu::initMinuterie()
{
    elapsed.start();
    frameTimer.start();
    tempsJeuMs = 0;

    timer.setInterval(1000 / 60);
    connect(&timer, &QTimer::timeout, this, &EcranJeu::tick);
    timer.start();
}

void EcranJeu::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);

    if (overlay && fadeInAnim) {
        overlay->setGeometry(rect());
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();
        fadeInAnim->stop();
        fadeInAnim->start();
    }

    if (!jeu) {
        jeu = new Jeu(size(), compteurPoints, compteurBalles, vies, configurationPartie.modeJeu, armes);

        switch (configurationPartie.difficulte) {
            case DifficultePartie::CHAOS:
                jeu->setMaxCiblesSimultanees(8);
                jeu->setFrequenceSpawn(450);
                jeu->setVariationFrequence(200);
                break;
            case DifficultePartie::RNG:
                // Effet de gameplay Ã  dÃ©finir
                break;
            case DifficultePartie::NORMAL:
            default:
                break;
        }

        jeu->setOnMoteurDemande([this]() {
            if (touches) touches->envoyerMoteur();
        });
    }

    if (touches) {
        touches->envoyerRaw("\n");
        touches->envoyerNbBalles(maxBalles);
    }

    tempsJeuMs = 0;
    frameTimer.restart();
}

void EcranJeu::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    arrierePlanCache = QPixmap();
    if (arrierePlan && !arrierePlan->isNull() && width() > 0 && height() > 0) {
        QPixmap scaled = arrierePlan->scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int x = std::max(0, (scaled.width()  - width())  / 2);
        int y = std::max(0, (scaled.height() - height()));
        arrierePlanCache = scaled.copy(QRect(x, y, width(), height()));
    }

    auto redimensionnerOverlay = [this](QWidget* w) {
        if (w) { w->setGeometry(rect()); w->raise(); }
    };
    redimensionnerOverlay(overlay);
    redimensionnerOverlay(overlayFadeOut);
    redimensionnerOverlay(menuPause);

    if (jeu) jeu->setTailleEcran(size());

    placerElementsGUI();
}

void EcranJeu::tick()
{
    if (enPause || transitionVersMenu) {
        frameTimer.restart();
        return;
    }

    qint64 deltaMs = frameTimer.restart();
    tempsJeuMs += deltaMs;

    if (jeu) jeu->update(tempsJeuMs);

    declencherFinPartie();

    if (gestionnaireEntrees)
        gestionnaireEntrees->lire(deltaMs);

    update();
}

void EcranJeu::keyPressEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat() || transitionVersMenu) { 
        e->ignore();
        return;
    }

    if (e->key() == Qt::Key_Escape) {
        enPause ? reprendreJeu() : mettreEnPause();
        e->accept();
        return;
    }
    if (e->key() == Qt::Key_R) {
        rechargerArme();
        e->accept();
        return;
    }

    QWidget::keyPressEvent(e);
}

void EcranJeu::mouseMoveEvent(QMouseEvent* event)
{
    if (!event || !reticule) {
        return;
    }
    if (configurationPartie.manette != TypeManette::CLAVIER_SOURIS) {
        event->ignore();
        return;
    }
    reticule->setPosition(event->pos());
}

void EcranJeu::mousePressEvent(QMouseEvent* event)
{
    if (enPause || transitionVersMenu) { 
        event->ignore();
        return;
    }

    if (configurationPartie.manette != TypeManette::CLAVIER_SOURIS) {
        event->ignore();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        tire();
    }
    else if (event->button() == Qt::RightButton) {
        Power();
    }
}

void EcranJeu::tire()
{
    if (!compteurBalles) {
        return;
    }

    int balles = compteurBalles->getBalles();

    if (balles <= 0) {
        if (gestionnaireAudio) {
            gestionnaireAudio->playSfx("gun_empty");
        }
        return;
    }

    bool cibleTouchee = jeu->Tirer(reticule->getX(), reticule->getY(), tempsJeuMs);

    compteurBalles->setBalles(balles - 1);
    emit ballesChanged(balles - 1);

    if (gestionnaireAudio) {
        gestionnaireAudio->playSfx(cibleTouchee ? "gunshot_target" : "gunshot");
    }

    declencherFinPartie();
}

void EcranJeu::rechargerArme()
{
    if (!compteurBalles) {
        return;
    }

    if (gestionnaireAudio) {
        gestionnaireAudio->playSfx("reload");
    }

    compteurBalles->setBalles(maxBalles);
    emit ballesChanged(maxBalles);
}

void EcranJeu::placerElementsGUI()
{
    const int largeurEcran = width();
    const int hauteurEcran = height();
    const int marge = int(largeurEcran * 0.01f);

    if (compteurBalles) {
        int largeurCible = std::clamp(int(largeurEcran * 0.12f), LARGEUR_MIN_BALLES, LARGEUR_MAX_BALLES);
        int largeurOriginale = compteurBalles->frameSize().width();

        if (largeurOriginale == 0) {
            return;
        }

        compteurBalles->setEchelle(float(largeurCible) / float(largeurOriginale));
        compteurBalles->move(marge, hauteurEcran - compteurBalles->height() - marge);
    }

    if (compteurPowerUp && compteurBalles) {
        QSize iconBase = compteurPowerUp->tailleIconBase();
        if (!iconBase.isEmpty()) {
            float s = float(compteurBalles->height()) / float(iconBase.height());
            compteurPowerUp->setEchelle(s);
        }

        int x = compteurBalles->x();
        int y = compteurBalles->y() - compteurPowerUp->height() - marge;
        compteurPowerUp->move(x, y);
        compteurPowerUp->raise();
    }

    if (compteurPoints && compteurBalles) {
        int hauteurPoints = compteurPoints->basePanelSize().height();
        if (hauteurPoints > 0) {
            compteurPoints->setEchelle(float(compteurBalles->height()) / float(hauteurPoints));
        }

        compteurPoints->move(largeurEcran - compteurPoints->width() - marge, hauteurEcran - compteurPoints->height() - marge);
    }

    if (vies && compteurBalles && compteurPoints) {
        const int borneGauche = compteurBalles->x() + compteurBalles->width() + marge;
        const int borneDroite = compteurPoints->x() - marge;
        const int largeurDisponible = borneDroite - borneGauche;

        if (largeurDisponible <= 0) {
            vies->move(marge, hauteurEcran - vies->height() - marge);
            vies->raise();
            return;
        }

        int hauteurCoeurs = vies->getTailleFrame().height();
        if (hauteurCoeurs > 0) {
            float s = float(int(compteurBalles->height() * 0.80f)) / float(hauteurCoeurs);
            vies->setEchelle(s);
        }
        if (vies->width() > largeurDisponible) {
            vies->setEchelle(vies->getEchelle() * float(largeurDisponible) / float(vies->width()));
        }

        int y = compteurBalles->y() + (compteurBalles->height() - vies->height()) / 2;
        int x = std::clamp((largeurEcran - vies->width()) / 2, borneGauche, borneDroite - vies->width());
        vies->move(x, y);
    }
}

void EcranJeu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (!arrierePlanCache.isNull()) {
        painter.drawPixmap(0, 0, arrierePlanCache);
    }
    else if (!arrierePlan.isNull()) {
        painter.drawPixmap(rect(), *arrierePlan);
    }
    else {
        painter.fillRect(rect(), Qt::black);
    }

    if (jeu) {
        jeu->dessiner(painter, tempsJeuMs);
    }

    // TestHitbox(painter);
}

void EcranJeu::TestHitbox(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(armes->choixArme(622, 300));
}

void EcranJeu::mettreEnPause()
{
    if (enPause) {
        return;
    }

    enPause = true;

    if (jeu) {
        jeu->setPause(true);
    }

    if (reticule) {
        reticule->hide();
    }

    unsetCursor();

    if (menuPause) {
        menuPause->hide();
        menuPause->deleteLater();
        menuPause = nullptr;
    }

    menuPause = new MenuPauseOverlay(gestionnaireAudio, this, touches);
    menuPause->setGeometry(rect());
    menuPause->show();
    menuPause->raise();

    connect(menuPause, &MenuPauseOverlay::reprendreDemande, this, &EcranJeu::reprendreJeu);
    connect(menuPause, &MenuPauseOverlay::retourMenuDemande, this, &EcranJeu::demarrerFadeOutVersMenu);
}

void EcranJeu::Power() {
    int incr = 0;
    bool cibleTouchee = false;

    if (power_up > 0) {
        rechargerArme();
        switch (armes->getPowerActuelle()) {
        case PowerUpType::GRENADE:
            cibleTouchee = jeu->PowerUp(reticule->getX(), reticule->getY(), tempsJeuMs);
            break;
        case PowerUpType::ZAP:
            jeu->TireGratuit(reticule->getX(), reticule->getY(), tempsJeuMs);
            incr = qMax(this->width(), this->height());
            for (int i = 1; i <= int(incr / 9); i++) {
                jeu->getArmes()->setMult(i);
                if (jeu->PowerUp(reticule->getX(), reticule->getY(), tempsJeuMs)) {
                    break;
                }
            }
            break;
        case PowerUpType::MITRAILLETTE:
            timer2 = new QTimer(this);
            compteur = 0;
            connect(timer2, &QTimer::timeout, this, [this]() {
                jeu->PowerUp(reticule->getX(), reticule->getY(), tempsJeuMs);
                compteur++;
                if (compteur >= 100) {
                    timer2->stop();
                }
                });
            timer2->start(100);
            break;
        case PowerUpType::TACTICAL_NUKE:
            cibleTouchee = jeu->PowerUp(reticule->getX(), reticule->getY(), tempsJeuMs);
            break;
        default:
            cibleTouchee = jeu->PowerUp(reticule->getX(), reticule->getY(), tempsJeuMs);
            break;
        }
    }

    if (gestionnaireAudio != nullptr && power_up > 0) {
        gestionnaireAudio->playSfx(cibleTouchee ? "gunshot_target" : "gunshot");
    }

    declencherFinPartie();
}

void EcranJeu::reprendreJeu()
{
    if (!enPause) {
        return;
    }

    enPause = false;

    if (jeu) {
        jeu->setPause(false);
    }

    if (menuPause) {
        menuPause->hide();
        menuPause->deleteLater();
        menuPause = nullptr;
    }

    frameTimer.restart();
    setCursor(Qt::BlankCursor);
    if (reticule) reticule->show();
    setFocus();
}

void EcranJeu::demarrerFadeOutVersMenu()
{
    if (transitionVersMenu) {
        return;
    }

    transitionVersMenu = true;
    enPause = true;

    if (jeu) {
        jeu->setPause(true);
    }

    if (menuPause) {
        menuPause->hide();
        menuPause->deleteLater();
        menuPause = nullptr;
    }

    if (!overlayFadeOut) {
        overlayFadeOut = new FadeOverlay(this);
    }

    overlayFadeOut->setGeometry(rect());
    overlayFadeOut->setAlpha(0);
    overlayFadeOut->show();
    overlayFadeOut->raise();

    // Animation du fondu Ã©cran
    if (!fadeOutAnim) {
        fadeOutAnim = new QPropertyAnimation(overlayFadeOut, "alpha", this);
        fadeOutAnim->setEasingCurve(QEasingCurve::InOutQuad);
    }
    fadeOutAnim->stop();
    fadeOutAnim->setDuration(800);
    fadeOutAnim->setStartValue(0);
    fadeOutAnim->setEndValue(255);

    // Animation du fondu musique
    if (gestionnaireAudio) {
        if (!fadeOutMusique) {
            fadeOutMusique = new QPropertyAnimation(gestionnaireAudio, "musicVolume", this);
            fadeOutMusique->setEasingCurve(QEasingCurve::InOutQuad);
        }
        fadeOutMusique->stop();
        fadeOutMusique->setDuration(800);
        fadeOutMusique->setStartValue(gestionnaireAudio->getMusicVolume());
        fadeOutMusique->setEndValue(0.0f);
        fadeOutMusique->start();
    }

    QObject::disconnect(fadeOutAnim, nullptr, this, nullptr);
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this]() { emit retourMenuDemande(); });
    fadeOutAnim->start();
}

void EcranJeu::declencherFinPartie()
{
    if (!vies || transitionVersMenu || enPause) {
        return;
    }

    if (vies->getDemiVies() > 0) {
        return;
    }

    transitionVersMenu = true;
    timer.stop();

    if (jeu) {
        jeu->setPause(true);
    }

    if (reticule) {
        reticule->hide();
    }

    if (!overlay) {
        overlay = new FadeOverlay(this);
    }
    overlay->setGeometry(rect());
    overlay->setAlpha(0);
    overlay->show();
    overlay->raise();

    if (!fadeInAnim) {
        fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
        fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    }
    fadeInAnim->stop();
    QObject::disconnect(fadeInAnim, nullptr, this, nullptr);
    fadeInAnim->setDuration(1000);
    fadeInAnim->setStartValue(0);
    fadeInAnim->setEndValue(255);
    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        int scoreFinal = compteurPoints ? compteurPoints->getPointsCible() : 0;
        emit finPartie(scoreFinal);
    });
    fadeInAnim->start();
}
