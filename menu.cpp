#include "menu.h"

MenuPrincipal::MenuPrincipal(QWidget *parent) : 
    QWidget(parent),
    arrierePlan(QDir::currentPath() + "/images/menu/background.png"),
    titreSprite(QDir::currentPath() + "/images/menu/titre.png")
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    panneau = new PanneauPrincipal(this);

    if (arrierePlan.isNull()) {
        std::cout << "MENU::Arrière plan non-chargé";
    }
    if (titreSprite.isNull()) {
        std::cout << "MENU::Titre non-chargé";
    }

    int decalageY = int(height() * ratioPanneaux);

    QRect zonePanneaux = rect();
    zonePanneaux.setTop(zonePanneaux.top() + decalageY);

    panneau->setGeometry(zonePanneaux);

    connect(panneau, &PanneauMenu::demanderJouer, this, []() {
        std::cout << "Start game!" << std::endl;
    });
    connect(panneau, &PanneauMenu::demanderScores, this, []() {
        std::cout << "Demande de scores!" << std::endl;
    });
    connect(panneau, &PanneauMenu::demanderOptions, this, []() {
        std::cout << "Demande d'options!" << std::endl;
    });
    connect(panneau, &PanneauMenu::demanderQuitter, this, []() {
        qApp->quit();
    });

    configuerAnimationTitre();

    cannettes = new DecorationMenu(this);

    cannettes->setSprite("/images/sprites/creme_menthe.png");
    cannettes->setCycle(1000);
    cannettes->setNombreDecorations(6);
    cannettes->setFPS(30);

    cannettes->show();

    cannettes->lower();

    reconstruireCacheArrierePlan();
}

MenuPrincipal::~MenuPrincipal() {
    delete panneau;
}

void MenuPrincipal::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    afficherArrierePlan(painter);

    afficherTitre(painter);

}

void MenuPrincipal::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    reconstruireCacheArrierePlan();

    if (!panneau) {
        return;
    }

    int decalageY = int(height() * ratioPanneaux);

    QRect zone(
        0,
        decalageY,
        width(),
        height() - decalageY
    );

    panneau->setGeometry(zone);

    if (!cannettes) return;

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
    if (arrierePlanCache.isNull()) {
        painter.fillRect(rect(), Qt::black);
        return;
    }

    painter.drawPixmap(0, 0, arrierePlanCache);
}

void MenuPrincipal::reconstruireCacheArrierePlan()
{
    if (arrierePlan.isNull() || width() <= 0 || height() <= 0) {
        arrierePlanCache = QPixmap();
        return;
    }

    const QPixmap scaled = arrierePlan.scaled(
        size(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );

    int x = (scaled.width() - width()) / 2;
    int y = (scaled.height() - height());
    x = std::max(0, x);
    y = std::max(0, y);

    const QRect crop(x, y, width(), height());
    arrierePlanCache = scaled.copy(crop);
}

void MenuPrincipal::afficherTitre(QPainter &painter) {
    if (titreSprite.isNull()) {
        return;
    }

    const int frameW = titreSprite.width() / nombreImageTitre;
    const int frameH = titreSprite.height();

    if (frameW > 0)
    {
        QRect src(indexImageTitre * frameW, 0, frameW, frameH);

        int drawW = int(width() * echelleTitre);
        int drawH = (drawW * frameH) / frameW;

        int drawX = (width() - drawW) / 2;
        int drawY = positionTitreY;

        painter.drawPixmap(QRect(drawX, drawY, drawW, drawH), titreSprite, src);
    }
    else
    {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Invalid titleFrameCount");
    }
}