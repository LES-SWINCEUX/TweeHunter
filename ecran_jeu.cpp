#include "ecran_jeu.h"

#include <QPainter>
#include <algorithm>

#include "Reticule.h"


EcranJeu::EcranJeu(QWidget* parent)
    : QWidget(parent)
{
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
}

void EcranJeu::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (overlay)
    {
        overlay->setGeometry(rect());
        overlay->raise();
    }
}

void EcranJeu::tick()
{
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
}

void EcranJeu::mouseMoveEvent(QMouseEvent* event)
{
    reticule->setPosition(event->pos());
}