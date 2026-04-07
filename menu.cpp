#include "menu.h"
#include "panneau_scores.h"

MenuPrincipal::MenuPrincipal(GestionnaireAudio* gestionnaireAudio, bool restartMusique, QWidget* parent, Touches* touches) :
    QWidget(parent),
    arrierePlan(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/background.png")),
    titreSprite(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/titre.png"))
{
    touchesPerso = touches;
    setAttribute(Qt::WA_OpaquePaintEvent);

    this->gestionnaireAudio = gestionnaireAudio;
    this->restartMusique = restartMusique;

    configuerAnimationTitre();

    cannettes = new DecorationMenu(this);

    cannettes->setSprite("/images/sprites/twisted_teas.png");
    cannettes->setCycle(1000);
    cannettes->setNombreImages(6);
    cannettes->setFPS(30);

    cannettes->show();
    cannettes->lower();

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->raise();
    overlay->hide();

    if (this->gestionnaireAudio != nullptr && restartMusique) {
        this->gestionnaireAudio->setPlaylist({
            QDir::currentPath() + "/sounds/menu/track_2.mp3",
            QDir::currentPath() + "/sounds/menu/track_1.mp3",
            QDir::currentPath() + "/sounds/menu/track_3.mp3"
        });

        this->gestionnaireAudio->setMusicVolumeAnimation(0.0f);
        this->gestionnaireAudio->playMusic();
    }

    estompeAnimation = new QPropertyAnimation(overlay, "alpha", this);
    estompeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(estompeAnimation, &QPropertyAnimation::finished, this, &MenuPrincipal::jouerDemande);

    connect(&timerManette, &QTimer::timeout, this, &MenuPrincipal::tickManette);
    timerManette.setInterval(16);
    timerManette.start();

    initialiserManette();

    afficherPanneauPrincipal();
}

MenuPrincipal::~MenuPrincipal() {
}

QRect MenuPrincipal::zonePanneauxBas() const
{
    const int decalageY = int(height() * ratioPanneaux);
    return QRect(0, decalageY, width(), std::max(0, height() - decalageY));
}

QRect MenuPrincipal::zonePourPanneau(PanneauMenu* p) const
{
    if (qobject_cast<PanneauScores*>(p) != nullptr) {
        return rect();
    }

    return zonePanneauxBas();
}

void MenuPrincipal::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    fadeEnCours = false;
    setEnabled(true);
    lancerFadeIn();
}

void MenuPrincipal::lancerFadeIn()
{
    if (overlay) {
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();

        if (!fadeInAnimation) {
            fadeInAnimation = new QPropertyAnimation(overlay, "alpha", this);
            fadeInAnimation->setEasingCurve(QEasingCurve::InOutQuad);
        }
        fadeInAnimation->stop();
        fadeInAnimation->setDuration(800);
        fadeInAnimation->setStartValue(255);
        fadeInAnimation->setEndValue(0);
        connect(fadeInAnimation, &QPropertyAnimation::finished, this, [this]() {
            overlay->hide();
            });
        fadeInAnimation->start();
    }
    
    if (!restartMusique) {
        return;
    }

    GestionnaireAudio* audio = this->gestionnaireAudio;
    if (audio != nullptr) {
        if (!fadeInMusique) {
            fadeInMusique = new QPropertyAnimation(audio, "musicVolume", this);
            fadeInMusique->setEasingCurve(QEasingCurve::InOutQuad);
        }
        fadeInMusique->stop();
        fadeInMusique->setDuration(800);
        fadeInMusique->setStartValue(0.0f);
        fadeInMusique->setEndValue(audio->getMusicVolumeSetting());
        fadeInMusique->start();
    }
}

void MenuPrincipal::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    afficherArrierePlan(painter);

    if (!cacherTitre) {
        afficherTitre(painter);
    }
}

void MenuPrincipal::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    arrierePlanCache = QPixmap();
    if (arrierePlan && !arrierePlan->isNull() && width() > 0 && height() > 0) {
        QPixmap scaled = arrierePlan->scaled(
            size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
        );

        int x = (scaled.width() - width()) / 2;
        int y = (scaled.height() - height());

        x = std::max(0, x);
        y = std::max(0, y);

        QRect crop(x, y, width(), height());
        arrierePlanCache = scaled.copy(crop);
    }

    if (panneau) {
        panneau->setGeometry(zonePourPanneau(panneau));
    }

    if (cannettes) {
        cannettes->setGeometry(rect());

        QVector<QRectF> zones = {
            {0.116f, 0.067f, 0.131f, 0.161f},
            {0.075f, 0.336f, 0.165f, 0.209f},
            {0.120f, 0.702f, 0.169f, 0.212f},

            {0.771f, 0.040f, 0.127f, 0.178f},
            {0.712f, 0.375f, 0.140f, 0.196f},
            {0.742f, 0.718f, 0.162f, 0.217f},
        };

        cannettes->setZones(zones);
    }

    if (overlay) {
        overlay->setGeometry(rect());
        overlay->raise();
    }
}

void MenuPrincipal::configuerAnimationTitre() {
    timerAnimationTitre.setInterval(tempsAnimation / 18);

    connect(&timerAnimationTitre, &QTimer::timeout, this, [this]() {

        if (animationActive)
        {
            indexImageTitre = (indexImageTitre + 1) % nombreImageTitre;
            imagesAffichees++;
            update();

            if (imagesAffichees < nombreImageTitre) {
                return;
            }

            animationActive = false;
            imagesAffichees = 0;
            timerPauseAnimation.restart();
        }
        else if (timerPauseAnimation.elapsed() >= tempsAttenteAnimation)
        {
            animationActive = true;
            indexImageTitre = 0;
        }
        });

    timerAnimationTitre.start();
}

void MenuPrincipal::afficherArrierePlan(QPainter& painter) {
    if (!arrierePlan || arrierePlan->isNull()) {
        return;
    }

    if (!arrierePlanCache.isNull()) {
        painter.drawPixmap(0, 0, arrierePlanCache);
        return;
    }

    painter.drawPixmap(rect(), *arrierePlan);
}

void MenuPrincipal::afficherTitre(QPainter& painter) {
    if (!titreSprite || titreSprite->isNull()) {
        return;
    }

    const int largeurImage = titreSprite->width() / nombreImageTitre;
    const int hauteurImage = titreSprite->height();
    if (largeurImage <= 0 || hauteurImage <= 0) {
        return;
    }

    QRect src(indexImageTitre * largeurImage, 0, largeurImage, hauteurImage);

    const float largeurMax = width() * 0.7f;
    const float hauteurMax = height() * 0.4f;

    float largeur = largeurMax;
    float hauteur = largeur * float(hauteurImage) / float(largeurImage);

    if (hauteur > hauteurMax) {
        hauteur = hauteurMax;
        largeur = hauteur * float(largeurImage) / float(hauteurImage);
    }

    const int positionX = (width() - int(largeur)) / 2;
    int positionY = int(height() * 0.04f) - int(height() * 0.06f);
    positionY = std::max(0, positionY);

    painter.drawPixmap(
        QRect(positionX, positionY, int(largeur), int(hauteur)),
        *titreSprite,
        src
    );
}

void MenuPrincipal::afficherOptions() {
    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauOptions(this->gestionnaireAudio, this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, [this]() {
        afficherPanneauPrincipal();
        // Index 2 = bouton Options (Jouer=0, Scores=1, Options=2, Quitter=3)
        panneau->focusSurIndex(2);
    });
}

void MenuPrincipal::afficherPanneauPrincipal() {
    cacherTitre = false;
    update();

    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauPrincipal(this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();

    connect(panneau, &PanneauMenu::demanderScores, this, &MenuPrincipal::afficherPanneauScores);
    connect(panneau, &PanneauMenu::demanderOptions, this, &MenuPrincipal::afficherOptions);

    connect(panneau, &PanneauMenu::demanderQuitter, this, []() {
        qApp->quit();
    });

    connect(panneau, &PanneauMenu::demanderJouer, this, [this]() {
        if (fadeEnCours) {
            return;
        }

        fadeEnCours = true;

        setEnabled(false);

        overlay->setAlpha(0);
        overlay->show();
        overlay->raise();

        estompeAnimation->stop();
        estompeAnimation->setDuration(1000);
        estompeAnimation->setStartValue(0);
        estompeAnimation->setEndValue(255);

        estompeAnimation->start();
    });
}

void MenuPrincipal::afficherPanneauScores() {
    cacherTitre = true;
    update();

    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauScores(this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, [this]() {
        afficherPanneauPrincipal();
        // Index 1 = bouton Scores (Jouer=0, Scores=1, Options=2, Quitter=3)
        panneau->focusSurIndex(1);
    });
}

void MenuPrincipal::keyPressEvent(QKeyEvent* e)
{
    if (!panneau) { QWidget::keyPressEvent(e); return; }

    switch (e->key()) {
    case Qt::Key_Up:   panneau->naviguerHaut(); break;
    case Qt::Key_Down: panneau->naviguerBas();  break;
    case Qt::Key_Return:
    case Qt::Key_Space: panneau->confirmer();   break;
    default: QWidget::keyPressEvent(e);
    }
}

void MenuPrincipal::initialiserManette()
{
    SDL_Init(SDL_INIT_GAMEPAD);

    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    if (ids && count > 0) {
        gamepad = SDL_OpenGamepad(ids[0]);
        SDL_free(ids);
    }
}

void MenuPrincipal::tickManette()
{
    if (!panneau || fadeEnCours) {
        return;
    }

    SDL_PumpEvents();

    if (!gamepad || !SDL_GamepadConnected(gamepad)) {
        if (gamepad) { 
            SDL_CloseGamepad(gamepad); 
            gamepad = nullptr; 
        }
        initialiserManette();
    }

    bool haut = false;
    bool bas = false;
    bool ok  = false;

    if (gamepad && SDL_GamepadConnected(gamepad)) {
        bool dpadHaut = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        bool dpadBas = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        bool joystickHaut = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) < -16000;
        bool joystickBas = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) >  16000;
        bool croix = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
        bool start = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);

        haut = dpadHaut || joystickHaut;
        bas = dpadBas || joystickBas;
        ok = croix || start;
    }

    if (haut && !dpadHautPrecedent) {
        panneau->naviguerHaut();
    }

    if (bas && !dpadBasPrecedent) {
        panneau->naviguerBas();
    }

    if (ok && !boutonOkPrecedent) {
        panneau->confirmer();
    }

    dpadHautPrecedent = haut;
    dpadBasPrecedent = bas;
    boutonOkPrecedent = ok;

    if (touchesPerso && touchesPerso->isJoystickPersoConnected()) {
        touchesPerso->lirePerso();

        int jy = touchesPerso->getyPerso();
        bool customHaut = (jy < 300);
        bool customBas = (jy > 700);
        bool customOk = touchesPerso->getGachette();

        if (customHaut && !customHautPrecedent) {
            panneau->naviguerHaut();
        }

        if (customBas && !customBasPrecedent) {
            panneau->naviguerBas();
        }

        if (customOk && !customOkPrecedent) {
            panneau->confirmer();
        }

        customHautPrecedent = customHaut;
        customBasPrecedent = customBas;
        customOkPrecedent = customOk;
    }
}