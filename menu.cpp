#include "menu.h"

MenuPrincipal::MenuPrincipal(QWidget *parent) : 
    QWidget(parent),
    arrierePlan(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/background.png")),
    titreSprite(SpriteManager::instance().getPixmap(QDir::currentPath() + "/images/menu/titre.png"))
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    if (!arrierePlan || arrierePlan->isNull()) {
        std::cout << "MENU::Arrière plan non-chargé";
    }
    if (!titreSprite || titreSprite->isNull()) {
        std::cout << "MENU::Titre non-chargé";
    }

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

    joueurMusique = new QMediaPlayer(this);
    musiqueMenu = new QAudioOutput(this);

    joueurMusique->setAudioOutput(musiqueMenu);
    joueurMusique->setSource(QUrl::fromLocalFile(QDir::currentPath() + "/sounds/menu/track_2.mp3"));
    musiqueMenu->setVolume(this->volumeMax);
    joueurMusique->play();

    estompeAnimation = new QPropertyAnimation(overlay, "alpha", this);
    estompeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    estompeMusique = new QPropertyAnimation(musiqueMenu, "volume");

    connect(estompeAnimation, &QPropertyAnimation::finished,
        this, &MenuPrincipal::jouerDemande);

    afficherPanneauPrincipal();
}

MenuPrincipal::~MenuPrincipal() {
}

void MenuPrincipal::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    afficherArrierePlan(painter);

    afficherTitre(painter);

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
        int decalageY = int(height() * ratioPanneaux);

        QRect zone(
            0,
            decalageY,
            width(),
            height() - decalageY
        );

        panneau->setGeometry(zone);
    }

    if (cannettes) {

        cannettes->setGeometry(rect());

        QVector<QRectF> zones = {
            {0.116f, 0.067f, 0.131f, 0.161f}, // haut-gauche
            {0.771f, 0.040f, 0.127f, 0.178f}, // haut-droite

            {0.075f, 0.336f, 0.165f, 0.209f}, // milieu-gauche
            {0.712f, 0.375f, 0.140f, 0.196f}, // milieu-droite

            {0.120f, 0.702f, 0.169f, 0.212f}, // bas-gauche
            {0.742f, 0.718f, 0.162f, 0.217f}, // bas-droite
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

void MenuPrincipal::afficherArrierePlan(QPainter &painter) {
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

    panneau = new PanneauOptions(musiqueMenu, this->volumeMax, this);

    panneau->setGeometry(ancienPanneau->geometry());

    ancienPanneau->hide();
    
    panneau->show();
    panneau->raise();

    ancienPanneau->deleteLater();

    connect(panneau, &PanneauMenu::demanderRetourOptions, this, &MenuPrincipal::afficherPanneauPrincipal);
}

void MenuPrincipal::afficherPanneauPrincipal() {
    PanneauMenu* ancienPanneau = panneau;

    panneau = new PanneauPrincipal(this);

    int decalageY = int(height() * ratioPanneaux);

    QRect zonePanneaux = rect();
    zonePanneaux.setTop(zonePanneaux.top() + decalageY);

    panneau->setGeometry(zonePanneaux);

    if (ancienPanneau != nullptr) {
        ancienPanneau->hide();

        panneau->show();
        panneau->raise();

        ancienPanneau->deleteLater();
    }

    connect(panneau, &PanneauMenu::demanderScores, this, []() {
        std::cout << "Demande de scores!" << std::endl;
        });

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

        estompeMusique->setDuration(1000);
        estompeMusique->setStartValue(musiqueMenu->volume());
        estompeMusique->setEndValue(0.0);

        estompeMusique->start(QAbstractAnimation::DeleteWhenStopped);

        estompeAnimation->start();
    });
}