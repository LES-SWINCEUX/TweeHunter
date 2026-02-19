#include "ecran_jeu.h"

#include <QPainter>
#include <algorithm>

#include "Reticule.h"


EcranJeu::EcranJeu(GestionnaireAudio* gestionnaireAudio, QWidget* parent)
    : QWidget(parent)
{
    this->gestionnaireAudio = gestionnaireAudio;
    if (gestionnaireAudio != nullptr) {
        gestionnaireAudio->stopAndClearMusic();
        gestionnaireAudio->setPlaylist({ QDir::currentPath() + "/sounds/jeu/track_1.mp3" });
        gestionnaireAudio->playMusic();
        estompeMusique = new QPropertyAnimation(this->gestionnaireAudio, "musicVolume");

        estompeMusique->setDuration(1000);
        estompeMusique->setStartValue(0.0);
        estompeMusique->setEndValue(this->gestionnaireAudio->getMusicVolumeSetting());

        estompeMusique->start(QAbstractAnimation::DeleteWhenStopped);
    }

    background = QPixmap(QDir::currentPath() + "/images/jeu/background.png");
    setAttribute(Qt::WA_OpaquePaintEvent);
    elapsed.start();

    timer.setInterval(1000 / 60);

    connect(&timer, &QTimer::timeout, this, [this]() {
        tick();
    });

    timer.start();

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


    
	//ajout à enlever apres test
    QPoint pos = QCursor::pos();
	pos = mapFromGlobal(pos);

    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
	reticule = new Reticule(this,pos,2); // création du réticule sur la sourie + choix du réticule
    reticule->show();

    //Partie joystick de test
    /*
    // --- 1. Initialiser SDL ---
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0)
    {
        qDebug() << "Erreur SDL:" << SDL_GetError();
        return;
    }
    
    // --- 2. Détecter les manettes ---
    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count); // Liste des gamepads
    qDebug() << "Nombre de manettes détectées:" << count;

    SDL_Gamepad* gamepad = nullptr;

    if (ids && count > 0)
    {
        // Ouvrir la première manette
        gamepad = SDL_OpenGamepad(ids[0]);
        if (gamepad)
            cout << "Manette ouverte !";
        else
            cout << "Erreur ouverture:" << SDL_GetError();

        SDL_free(ids); // libérer la mémoire retournée par SDL_GetGamepads
    }
    else {
        cout << "Nope" << endl;

    }
    
    
    // --- 3. Lire le joystick régulièrement ---
    QTimer* timer = new QTimer(this);
    timer->start(16); // ~60 Hz

    connect(timer, &QTimer::timeout, this, [=]() {
        if (!gamepad) return;

        SDL_UpdateGamepads();

        float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
        float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

        // Deadzone
        if (std::fabs(x) < 0.1f) x = 0;
        if (std::fabs(y) < 0.1f) y = 0;

        qDebug() << "X:" << x << "Y:" << y;
        });

    */
    //Fin de la zone de test
    
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0)
{
    qDebug() << "Erreur SDL:" << SDL_GetError();
}

    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        overlay->hide();
        });
}

EcranJeu::~EcranJeu()
{
    delete jeu;
}

void EcranJeu::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);

    if (overlay && fadeInAnim)
    {
        overlay->setGeometry(rect());
        overlay->setAlpha(255);
        overlay->show();
        overlay->raise();

        fadeInAnim->stop();
        fadeInAnim->start();
    }
    if (!jeu) {
        jeu = new Jeu(size());
    }
}

void EcranJeu::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (overlay)
    {
        overlay->setGeometry(rect());
        overlay->raise();
    }
    if(jeu) {
        jeu->setTailleEcran(size());
	}
}

void EcranJeu::tick()
{
    if (jeu) {
		jeu->update(elapsed.elapsed());
    }
    update();
}

void EcranJeu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (!background.isNull()) {
        painter.drawPixmap(rect(), background);
    }
    else {
        painter.fillRect(rect(), Qt::black);
    }
    if (jeu) {
		jeu->dessiner(painter, elapsed.elapsed()); 
    }
}

void EcranJeu::mouseMoveEvent(QMouseEvent* event)
{
    reticule->setPosition(event->pos());
}