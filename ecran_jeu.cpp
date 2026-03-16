#include "ecran_jeu.h"

EcranJeu::EcranJeu(GestionnaireAudio* gestionnaireAudio, QWidget* parent,int arme, Touches* t)
    : QWidget(parent)
{
    //ajout à enlever apres test
    QPoint pos = QCursor::pos();
    pos = mapFromGlobal(pos);

    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
    reticule = new Reticule(this, pos, arme,t); // création du réticule sur la sourie + choix du réticule
    reticule->show();

    // Stocker la référence aux touches pour l'envoi série
    touches = t;

    armes = new Armes(arme);
    maxBalles = armes->nbMunitions();

    //compteur de balle, points et vies

    compteurBalles = new CompteurBalles(this, armes->nbMunitions());
    vies = new Vies(this);
    compteurPoints = new CompteurPoints(this);

    // Connecter le signal de changement de balles à l'envoi JSON série
    if (touches) {
        connect(compteurBalles, &CompteurBalles::ballesChanged,
                touches, &Touches::envoyerNbBalles);
    }

    compteurBalles->move(20, height() - compteurBalles->height() + 120);
    compteurBalles->setBalles(armes->nbMunitions());
    compteurBalles->show();

    vies->setVies(3);
    vies->move(20, 20);
    vies->show();

    compteurPoints->setNombresNumeros(6);
    compteurPoints->setPoints(0);
    compteurPoints->setAnimation(true);
    compteurPoints->setVitesseAnimation(2, 1);
    compteurPoints->show();

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

        gestionnaireAudio->addSfx("gunshot", QDir::currentPath() + "/sounds/sfx/gunshot.wav", this->maxBalles);
        gestionnaireAudio->addSfx("gunshot_target", QDir::currentPath() + "/sounds/sfx/gunshot_target.wav", this->maxBalles);
        gestionnaireAudio->addSfx("gun_empty", QDir::currentPath() + "/sounds/sfx/gun_empty.wav", this->maxBalles);
        gestionnaireAudio->addSfx("reload", QDir::currentPath() + "/sounds/sfx/reload.wav", this->maxBalles);
    }

    arrierePlan = SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/jeu/background.png");
    setAttribute(Qt::WA_OpaquePaintEvent);
    elapsed.start();
    frameTimer.start();
    tempsJeuMs = 0;

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

    //activation de sdl pour les manettes
    cout << "Initialisation de SDL3" << endl;

    if (SDL_Init(SDL_INIT_GAMEPAD) < 0) {
        qDebug() << "Erreur SDL:" << SDL_GetError();
    }

    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        overlay->hide();
        });


    gamepad = reticule->getGamepad(); //récupération du controle de lamanette pour le tir

    /*
    QTimer* timer = new QTimer(this);
    this->timerManette = timer;
    timer->start(16); // ~60 Hz

    connect(timer, &QTimer::timeout, this, [=]() {// prise des données du joystick
        if (enPause || transitionVersMenu) {
            return;
        }
        if (reticule->tirer()) {

            if (!gachettePrecedente) {
                gachettePrecedente = true;
                tire();

            }
        }
        else{
            gachettePrecedente = false;
        }
    });*/

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

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
        jeu = new Jeu(size(), compteurPoints, compteurBalles, vies, ModeJeu::PLUS_18, armes);
    }

    // Envoyer le nombre de balles initial à l'Arduino au démarrage de la partie
    if (touches) {
        touches->envoyerRaw("\n");
        touches->envoyerNbBalles(maxBalles);
    }
    // Réinitialise le temps de jeu (utile si on revient sur l'écran)
    tempsJeuMs = 0;
    frameTimer.restart();
}

void EcranJeu::keyPressEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat()) {
        e->ignore();
        return;
    }

    if (transitionVersMenu) {
        e->ignore();
        return;
    }

    if (e->key() == Qt::Key_Escape)
    {
        if (enPause) {
            reprendreJeu();
        }
        else {
            mettreEnPause();
        }
        e->accept();
        return;
    }

    if (e->key() == Qt::Key_R)
    {
        rechargerArme();

        e->accept();
        return;
    }

    QWidget::keyPressEvent(e);
}

void EcranJeu::mousePressEvent(QMouseEvent* event) {
    if (enPause || transitionVersMenu) {
        event->ignore();
        return;
    }
    cout << "Tire avec la souris" << endl;
    tire();
}

void EcranJeu::resizeEvent(QResizeEvent* e)
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

    if (overlay)
    {
        overlay->setGeometry(rect());
        overlay->raise();
    }
    if (overlayFadeOut)
    {
        overlayFadeOut->setGeometry(rect());
        overlayFadeOut->raise();
    }
    if (menuPause)
    {
        menuPause->setGeometry(rect());
        menuPause->raise();
    }
    if (jeu) {
        jeu->setTailleEcran(size());
    }

    placerElementsGUI();
}

void EcranJeu::tick()
{
    if (enPause || transitionVersMenu) {
        frameTimer.restart();
        return;
    }

    // Sauvegarde le delta AVANT tout le reste pour eviter que restart() retourne ~0ms
    qint64 deltaMs = frameTimer.restart();

    if (jeu) {
        tempsJeuMs += deltaMs;
        jeu->update(tempsJeuMs);
    }

    SDL_Event event;
    if (SDL_PollEvent(&event)) // s'active s'il y a un changement sur les imputs de la manette
    {
        SDL_PumpEvents();
        if (reticule->tirer()) {

            if (!gachettePrecedente) {
                gachettePrecedente = true;
                tire();

            }
        }
        else {
            gachettePrecedente = false;
        }
    }

    if (reticule->getTouches()->isJoystickPersoConnected()) {

        reticule->getTouches()->lirePerso(); // met à jour les données de la mannette personalisée

        if (reticule->getTouches()->getGachette()) {

            if (!gachettePrecedente) {
                gachettePrecedente = true;
                tire();

            }
        }
        else {
            gachettePrecedente = false;
        }

        if (reticule->getTouches()->getReload() && reticule->getTouches()->getAccelerometre()) {
            rechargerArme();
        }

        if (reticule->getTouches()->getReload() && (reticule->getTouches()->getEncodeur()!=0)) {
            cout << "Pause du jeu" << endl;
            mettreEnPause();
        }
        
	}
    



        // Applique le mouvement joystick avec le delta-time correct
     reticule->applyJoystickPerso(this, (float)deltaMs);
    
    

    update();
}

void EcranJeu::mettreEnPause()
{
    if (enPause) return;
    enPause = true;

    // Stop la logique de jeu (sans arrêter la boucle de tick).
    if (jeu) {
        jeu->setPause(true);
    }

    // Cache le réticule et remet un curseur normal pour cliquer sur les boutons.
    if (reticule) {
        reticule->hide();
    }

    unsetCursor();

    menuPause = new MenuPauseOverlay(this->gestionnaireAudio, this);
    menuPause->setGeometry(rect());
    menuPause->show();
    menuPause->raise();

    connect(menuPause, &MenuPauseOverlay::reprendreDemande, this, &EcranJeu::reprendreJeu);
    connect(menuPause, &MenuPauseOverlay::retourMenuDemande, this, [this]() {
        demarrerFadeOutVersMenu();
        });
}

void EcranJeu::reprendreJeu()
{
    if (!enPause) return;
    enPause = false;
    if (jeu) jeu->setPause(false);

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

    if (menuPause) {
        menuPause->hide();
        menuPause->deleteLater();
        menuPause = nullptr;
    }

    enPause = true;
    if (jeu) {
        jeu->setPause(true);
    }

    if (!overlayFadeOut) {
        overlayFadeOut = new FadeOverlay(this);
    }
    overlayFadeOut->setGeometry(rect());
    overlayFadeOut->setAlpha(0);
    overlayFadeOut->show();
    overlayFadeOut->raise();

    if (!fadeOutAnim) {
        fadeOutAnim = new QPropertyAnimation(overlayFadeOut, "alpha", this);
        fadeOutAnim->setEasingCurve(QEasingCurve::InOutQuad);
    }
    fadeOutAnim->stop();
    fadeOutAnim->setDuration(800);
    fadeOutAnim->setStartValue(0);
    fadeOutAnim->setEndValue(255);

    if (gestionnaireAudio != nullptr) {
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
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this]() {
        emit retourMenuDemande();
        });

    fadeOutAnim->start();
}

void EcranJeu::placerElementsGUI()
{
    int largeurEcran = width();
    int hauteurEcran = height();

    float ratioMarges = 0.01f;

    int marge = int(largeurEcran * ratioMarges);

    if (compteurBalles) {
        float ratioLargeur = 0.12f;

        int largeurCible = int(largeurEcran * ratioLargeur);

        largeurCible = std::clamp(largeurCible, largeurMinBalles, largeurMaxBalles);

        int largeurOriginale = compteurBalles->frameSize().width();

        if (largeurOriginale == 0) {
            return;
        }

        float echelle = float(largeurCible) / float(largeurOriginale);
        compteurBalles->setEchelle(echelle);

        int x = marge;
        int y = hauteurEcran - compteurBalles->height() - marge;

        compteurBalles->move(x, y);
    }

    if (compteurPoints && compteurBalles) {
        int hauteurBalles = compteurBalles->height();
        int hauteurPoints = compteurPoints->basePanelSize().height();

        if (hauteurPoints > 0)
        {
            float echellePoints = float(hauteurBalles) / float(hauteurPoints);
            compteurPoints->setEchelle(echellePoints);
        }

        int x = largeurEcran - compteurPoints->width() - marge;
        int y = hauteurEcran - compteurPoints->height() - marge;

        compteurPoints->move(x, y);
    }
    if (vies && compteurBalles && compteurPoints)
    {
        int borneGauche = compteurBalles->x() + compteurBalles->width() + marge;
        int borneDroite = compteurPoints->x() - marge;

        int largeurDisponible = borneDroite - borneGauche;

        if (largeurDisponible <= 0)
        {
            int y = hauteurEcran - vies->height() - marge;
            vies->move(marge, y);
            vies->raise();
            return;
        }

        int hauteurCible = int(compteurBalles->height() * 0.80f);
        int hauteurCoeurs = vies->getTailleFrame().height();
        if (hauteurCoeurs > 0)
        {
            float s = float(hauteurCible) / float(hauteurCoeurs);
            vies->setEchelle(s);
        }

        if (vies->width() > largeurDisponible)
        {
            float fit = float(largeurDisponible) / float(vies->width());
            vies->setEchelle(vies->getEchelle() * fit);
        }

        int y = compteurBalles->y() + (compteurBalles->height() - vies->height()) / 2;

        int x = (largeurEcran - vies->width()) / 2;

        int minX = borneGauche;
        int maxX = borneDroite - vies->width();
        if (maxX < minX) {
            x = minX;
        }
        else {
            x = std::clamp(x, minX, maxX);
        }

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


    //Test de dessin du cercle de collision du tir

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(armes->choixArme(reticule->getArme(), 622, 300));
    //painter.drawPath(armes->choixArme(5, 622, 300));
    //painter.drawPath(armes->choixArme(4, reticule->getX(), reticule->getY()));


}

void EcranJeu::mouseMoveEvent(QMouseEvent* event)
{
    reticule->setPosition(event->pos());
}

void EcranJeu::tire() {
    //cout << "Tire détecté à la position x:" << reticule->getX() << " y:" << reticule->getY() << endl;
    int nombreBalles = 0;
    int nombreVies = vies->getDemiVies();

    if (compteurBalles != nullptr) {
        nombreBalles = compteurBalles->getBalles();
    }

    bool cibleTouchee = jeu->Tirer(reticule->getX(), reticule->getY(), tempsJeuMs, reticule->getChoixTir());

    if (gestionnaireAudio != nullptr && nombreBalles > 0) {
        gestionnaireAudio->playSfx(cibleTouchee ? "gunshot_target" : "gunshot");
    }

    if (gestionnaireAudio != nullptr && nombreBalles <= 0) {
        gestionnaireAudio->playSfx("gun_empty");
    }

    if (vies->getDemiVies() > 0) {
        return;
    }

    timer.stop();

    overlay = new FadeOverlay(this);
    overlay->setGeometry(rect());
    overlay->setAlpha(0);
    overlay->show();
    overlay->raise();

    fadeInAnim = new QPropertyAnimation(overlay, "alpha", this);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    fadeInAnim->setDuration(1000);
    fadeInAnim->setStartValue(0);
    fadeInAnim->setEndValue(255);

    connect(fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        int scoreFinal = compteurPoints ? compteurPoints->getPointsCible() : 0;
        emit finPartie(scoreFinal);
        });

    fadeInAnim->start();
}

void EcranJeu::rechargerArme() {
    if (!compteurBalles) {
        return;
    }

    if (gestionnaireAudio) {
        gestionnaireAudio->playSfx("reload");
    }

    compteurBalles->setBalles(this->maxBalles);
}