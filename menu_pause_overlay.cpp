#include "menu_pause_overlay.h"

MenuPauseOverlay::MenuPauseOverlay(GestionnaireAudio* gestionnaireAudio, QWidget* parent, Touches* touchesParam)
    : QWidget(parent), titreSprite(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/titre.png"))
{
    touches = touchesParam;
    this->gestionnaireAudio = gestionnaireAudio;
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    configuerAnimationTitre();

    connect(&timerManette, &QTimer::timeout, this, [this]() {
        if (!panneau) {
            return;
        }

        if (touches) {
            const bool avantConnectee = touches->isAnyConnected();
            touches->verifierConnexion();
            const bool apresConnectee = touches->isAnyConnected();
            if (avantConnectee && !apresConnectee) {
                panneau->reinitialiserFocus();
            }
            touches->lireNavigation();
        }
    });
    timerManette.setTimerType(Qt::PreciseTimer);
    timerManette.setInterval(16);

    if (touches) {
        connect(touches, &Touches::naviguerRetour, this, [this]() {
            emit reprendreDemande();
        });
    }

    // Délai avant de démarrer le polling pour éviter de capter le bouton Start
    // encore enfoncé au moment où le menu de pause s'ouvre.
    QTimer::singleShot(200, this, [this]() { timerManette.start(); });

    afficherPanneauPrincipal();
}

QRect MenuPauseOverlay::zonePanneauxBas() const
{
    const int decalageY = int(height() * RATIO_PANNEAUX);
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

    if (!panneau) { 
        QWidget::keyPressEvent(e);
        return; 
    }

    switch (e->key()) {
    case Qt::Key_Up:
        panneau->naviguerHaut();
        break;
    case Qt::Key_Down:
        panneau->naviguerBas();  
        break;
    case Qt::Key_Return:
    case Qt::Key_Space: 
        panneau->confirmer();
        break;
    default: QWidget::keyPressEvent(e);
    }
}

bool MenuPauseOverlay::manetteConnectee() const
{
    if (!touches) {
        return false;
    }

    return touches->isJoystickConnected() || touches->isJoystickPersoConnected();
}

void MenuPauseOverlay::initialiserManette()
{
    if (touches) {
        touches->verifierConnexion();
    }
}

void MenuPauseOverlay::configuerAnimationTitre() {
    timerAnimationTitre.setTimerType(Qt::PreciseTimer);
    timerAnimationTitre.setInterval(INTERVALE_TITRE_MS);

    connect(&timerAnimationTitre, &QTimer::timeout, this, [this]() {

        if (animationActive)
        {
            indexImageTitre = (indexImageTitre + 1) % NOMBRE_IMAGE_TITRE;
            imagesAffichees++;
            update();

            if (imagesAffichees < NOMBRE_IMAGE_TITRE) {
                return;
            }

            animationActive = false;
            imagesAffichees = 0;
            timerPauseAnimation.restart();
        }
        else if (timerPauseAnimation.elapsed() >= TEMPS_ATTENTE_ANIMATION)
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

    panneau = new PanneauPausePrincipal(manetteConnectee(), this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();
    panneau->connecterTouches(touches);

    connect(panneau, &PanneauMenu::demanderScores, this, &MenuPauseOverlay::afficherPanneauScores);
    connect(panneau, &PanneauMenu::demanderOptions, this, &MenuPauseOverlay::afficherOptions);
    connect(panneau, &PanneauMenu::demanderQuitter, this, &MenuPauseOverlay::retourMenuDemande);
    connect(panneau, &PanneauMenu::demanderJouer, this, &MenuPauseOverlay::reprendreDemande);
}

void MenuPauseOverlay::afficherOptions() {
    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauOptions(this->gestionnaireAudio, manetteConnectee(), this);
    panneau->setGeometry(zonePourPanneau(panneau));

    if (ancienPanneau) {
        ancienPanneau->hide();
        ancienPanneau->deleteLater();
    }

    panneau->show();
    panneau->raise();
    panneau->connecterTouches(touches);

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, [this]() {
        afficherPanneauPrincipal();
        // Index 2 = bouton Options (Jouer=0, Scores=1, Options=2, Quitter=3)
        if (manetteConnectee()) {
            panneau->focusSurIndex(2);
        }
    });
}

void MenuPauseOverlay::afficherPanneauScores()
{
    cacherTitre = true;
    update();

    PanneauMenu* ancien = panneau;

    panneau = new PanneauScores(manetteConnectee(), this);
    panneau->setGeometry(rect());
    panneau->show();
    panneau->raise();
    panneau->connecterTouches(touches);

    if (ancien) {
        ancien->hide();
        ancien->deleteLater();
    }

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, [this]() {
        afficherPanneauPrincipal();
        // Index 1 = bouton Scores (Jouer=0, Scores=1, Options=2, Quitter=3)
        if (manetteConnectee()) {
            panneau->focusSurIndex(1);
        }
    });
}

void MenuPauseOverlay::afficherTitre(QPainter& painter) {
    if (!titreSprite || titreSprite->isNull()) {
        return;
    }

    const int largeurImage = titreSprite->width() / NOMBRE_IMAGE_TITRE;
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

	rectTitre = QRect(positionX, positionY, int(largeur), int(hauteur));

    painter.drawPixmap(rectTitre, *titreSprite, src);
}

void MenuPauseOverlay::mousePressEvent(QMouseEvent* event)
{
    if (rectTitre.contains(event->pos())) {
        ouvrirEasterEggTitre();
        return;
            
    }
    QWidget::mousePressEvent(event);

}

void MenuPauseOverlay::ouvrirEasterEggTitre() 
{
	EasterEggDialog* dialog = new EasterEggDialog(6, this);
    dialog->show();
}
