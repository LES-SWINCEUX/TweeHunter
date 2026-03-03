#include "menu_pause_overlay.h"

#include <QKeyEvent>

#include "panneau_pause_principal.h"

MenuPauseOverlay::MenuPauseOverlay(GestionnaireAudio* gestionnaireAudio, QWidget* parent)
    : QWidget(parent), titreSprite(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/titre.png"))
{
    this->gestionnaireAudio = gestionnaireAudio;

    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    configuerAnimationTitre();

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

    QWidget::keyPressEvent(e);
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

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, &MenuPauseOverlay::afficherPanneauPrincipal);
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

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, &MenuPauseOverlay::afficherPanneauPrincipal);
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
