#include "menu_pause_overlay.h"

#include <QKeyEvent>

#include "panneau_pause_principal.h"

MenuPauseOverlay::MenuPauseOverlay(GestionnaireAudio* gestionnaireAudio, QWidget* parent, Touches* touches)
    : QWidget(parent), titreSprite(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/titre.png"))
{
    touchesPerso = touches;
    this->gestionnaireAudio = gestionnaireAudio;

    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    configuerAnimationTitre();

    // Timer poll manette SDL ~60 Hz
    connect(&timerManette, &QTimer::timeout, this, &MenuPauseOverlay::tickManette);
    timerManette.setInterval(16);
    timerManette.start();

    initialiserManette();

    afficherPanneauPrincipal();
}

QRect MenuPauseOverlay::zonePanneauxBas() const
{
    const int decalageY = int(height() * ratioPanneaux);
    return QRect(0, decalageY, width(), std::max(0, height() - decalageY));
}

QRect MenuPauseOverlay::zonePourPanneau(PanneauMenu* p) const
{
    if (qobject_cast<PanneauScores*>(p) != nullptr) {
        return rect();
    }

    return zonePanneauxBas();
}

void MenuPauseOverlay::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 170));

    if (!cacherTitre) {
        afficherTitre(p);
    }
}

void MenuPauseOverlay::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (panneau) {
        panneau->setGeometry(zonePourPanneau(panneau));
    }
}

void MenuPauseOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat()) {
        e->ignore();
        return;
    }

    if (e->key() == Qt::Key_Escape) {
        emit reprendreDemande();
        e->accept();
        return;
    }

    if (!panneau) { QWidget::keyPressEvent(e); return; }

    switch (e->key()) {
    case Qt::Key_Up:    panneau->naviguerHaut(); break;
    case Qt::Key_Down:  panneau->naviguerBas();  break;
    case Qt::Key_Return:
    case Qt::Key_Space: panneau->confirmer();    break;
    default: QWidget::keyPressEvent(e);
    }
}

void MenuPauseOverlay::initialiserManette()
{
    SDL_Init(SDL_INIT_GAMEPAD);

    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    if (ids && count > 0) {
        gamepad = SDL_OpenGamepad(ids[0]);
        SDL_free(ids);
    }
}

void MenuPauseOverlay::tickManette()
{
    if (!panneau) return;

    SDL_PumpEvents();

    // --- Manette SDL (PlayStation) ---
    if (!gamepad || !SDL_GamepadConnected(gamepad)) {
        if (gamepad) { SDL_CloseGamepad(gamepad); gamepad = nullptr; }
        initialiserManette();
    }

    bool haut   = false;
    bool bas    = false;
    bool ok     = false;
    bool retour = false;

    if (gamepad && SDL_GamepadConnected(gamepad)) {
        bool dpadHaut     = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        bool dpadBas      = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        bool joystickHaut = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) < -16000;
        bool joystickBas  = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) >  16000;
        bool croix        = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
        bool rond         = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST);

        haut   = dpadHaut  || joystickHaut;
        bas    = dpadBas   || joystickBas;
        ok     = croix;      // Start retiré : il est réservé à l'ouverture/fermeture de la pause
        retour = rond;
    }

    if (haut   && !dpadHautPrecedent)  panneau->naviguerHaut();
    if (bas    && !dpadBasPrecedent)   panneau->naviguerBas();
    if (ok     && !boutonOkPrecedent)  panneau->confirmer();
    if (retour && !boutonOkPrecedent)  emit reprendreDemande();

    dpadHautPrecedent = haut;
    dpadBasPrecedent  = bas;
    boutonOkPrecedent = ok || retour;

    // --- Manette custom (série) ---
    if (touchesPerso && touchesPerso->isJoystickPersoConnected()) {
        touchesPerso->lirePerso();

        int jy = touchesPerso->getyPerso();
        bool customHaut = (jy > 700);
        bool customBas  = (jy < 300);
        bool customOk   = touchesPerso->getGachette();

        if (customHaut && !customHautPrecedent) panneau->naviguerHaut();
        if (customBas  && !customBasPrecedent)  panneau->naviguerBas();
        if (customOk   && !customOkPrecedent)   panneau->confirmer();

        customHautPrecedent = customHaut;
        customBasPrecedent  = customBas;
        customOkPrecedent   = customOk;
    }
}

void MenuPauseOverlay::configuerAnimationTitre() {
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

void MenuPauseOverlay::afficherPanneauPrincipal() {
    cacherTitre = false;
    update();

    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauPausePrincipal(this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();

    connect(panneau, &PanneauMenu::demanderScores, this, &MenuPauseOverlay::afficherPanneauScores);
    connect(panneau, &PanneauMenu::demanderOptions, this, &MenuPauseOverlay::afficherOptions);
    connect(panneau, &PanneauMenu::demanderQuitter, this, &MenuPauseOverlay::retourMenuDemande);
    connect(panneau, &PanneauMenu::demanderJouer, this, &MenuPauseOverlay::reprendreDemande);
}

void MenuPauseOverlay::afficherOptions() {
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

void MenuPauseOverlay::afficherPanneauScores()
{
    cacherTitre = true;
    update();

    PanneauMenu* ancien = panneau;

    panneau = new PanneauScores(this);
    panneau->setGeometry(rect());
    panneau->show();
    panneau->raise();

    if (ancien) {
        ancien->hide();
        ancien->deleteLater();
    }

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, [this]() {
        afficherPanneauPrincipal();
        // Index 1 = bouton Scores (Jouer=0, Scores=1, Options=2, Quitter=3)
        panneau->focusSurIndex(1);
    });
}

void MenuPauseOverlay::afficherTitre(QPainter& painter) {
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
