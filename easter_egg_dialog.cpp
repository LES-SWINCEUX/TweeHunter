#include "easter_egg_dialog.h"

const QStringList& EasterEggDialog::messages()
{
    static const QStringList msgs = {
        // Canette 0 – haut-gauche
        "Cette canette a un prénom.\nOn l'appelle Twee.\nTwee te juge.",

        // Canette 1 – milieu-gauche
        "Bravo.\nT'as cliqué sur une canette.\nC'est le plus loin que t'es allé aujourd'hui.",

        // Canette 2 – bas-gauche
        "Félicitations!\nTu viens de trouver rien du tout.\n(Ou presque.)",

        // Canette 3 – haut-droite
        "Cette canette te regarde.\nElle est déçue.\nToi aussi tu le seras.",

        // Canette 4 – milieu-droite
        "Secret déverrouillé :\nRien. Absolument rien.\nBonne journée.",

        // Canette 5 – bas-droite
        "Tu viens de perdre 3 secondes.\nNous aussi à écrire ça.\nOn est quitte.",

        // Titre Kiwi
		"Wow une belle tete de Kiwi.\n Prends le pas comme un compliment"
    };
    return msgs;
}

EasterEggDialog::EasterEggDialog(int indexCanette, QWidget* parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint),
    indexCanette(indexCanette)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);
    setWindowTitle("Twisted Tea");
    setFixedSize(520, 400);
    setMaximumSize(520, 400);
    setMinimumSize(520, 400);

    spriteCanette = SpriteManager::instance().getPixmap(
        QDir::currentPath() + "/images/sprites/twisted_teas.png"
    );

    connect(&timerAnim, &QTimer::timeout, this, [this]() { update(); });
    timerAnim.setInterval(16);
    timerAnim.start();

    tempsOuverture.start();
}

void EasterEggDialog::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const qint64 ms = enFermeture ? tempsFermeture : tempsOuverture.elapsed();

    float tPop = 1.0f;
    float alpha = 1.0f;

    if (enFermeture) {
        float t = float(ms) / float(DUREE_FERMETURE_MS);
        t = std::min(t, 1.0f);
        alpha = 1.0f - t;
        tPop = 1.0f - t * 0.15f;
        if (t >= 1.0f) {
            close();
            return;
        }
    }
    else {
        float t = float(ms) / float(DUREE_POP_MS);
        t = std::min(t, 1.0f);
        float overshoot = 1.0f + 0.25f * qSin(t * M_PI) * (1.0f - t);
        tPop = t * overshoot;
        alpha = t;
    }

    p.fillRect(rect(), QColor(0, 0, 0, int(180 * alpha)));

    const int COLS = 4, ROWS = 3;
    const int TOTAL_FRAMES = COLS * ROWS;

    int frame = 0;
    if (spriteCanette && !spriteCanette->isNull()) {
        const int decalage = indexCanette * 2;
        frame = int((tempsOuverture.elapsed() / (1000 / TOTAL_FRAMES) + decalage)) % TOTAL_FRAMES;
    }

    const int cx = width() / 2;
    const int cy = int(height() * 0.38f);
    const float cibleHauteurBase = height() * 0.38f * tPop;

    if (spriteCanette && !spriteCanette->isNull()) {
        const int fw = spriteCanette->width() / COLS;
        const int fh = spriteCanette->height() / ROWS;
        const int col = frame % COLS;
        const int row = frame / COLS;
        QRect src(col * fw, row * fh, fw, fh);

        const float ratio = float(fw) / float(fh);
        const float cibleHauteur = cibleHauteurBase;
        const float cibleLargeur = cibleHauteur * ratio;

        QRect destCanette(
            int(cx - cibleLargeur / 2),
            int(cy - cibleHauteur / 2),
            int(cibleLargeur),
            int(cibleHauteur)
        );

        p.setOpacity(alpha);
        p.drawPixmap(destCanette, *spriteCanette, src);
        p.setOpacity(1.0f);
    }

    const QString& msg = (indexCanette >= 0 && indexCanette < messages().size()) ? messages()[indexCanette] : "...";

    QFont font;
    font.setFamily("Luckiest Guy");
    font.setPixelSize(int(height() * 0.05f));
    p.setFont(font);

    QFontMetrics fm(font);
    const int lineH = fm.height() + 4;
    const QStringList lignes = msg.split('\n');
    const int bulleHauteur = lignes.size() * lineH + 32;

    int largeurMax = 0;
    for (const QString& ligne : lignes) {
        largeurMax = std::max(largeurMax, fm.horizontalAdvance(ligne));
    }
    const int bulleLargeur = std::min(largeurMax + 48, int(width() * 0.90f));
    const int bulleX = (width() - bulleLargeur) / 2;
    const int bulleY = cy + int(cibleHauteurBase / 2) + 24;

    const int rayon = 18;
    QRect rectBulle(bulleX, bulleY, bulleLargeur, bulleHauteur);

    p.setOpacity(alpha * 0.35f);
    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rectBulle.translated(4, 6), rayon, rayon);

    p.setOpacity(alpha);
    p.setBrush(QColor(255, 230, 60, 230));
    p.setPen(QPen(QColor(200, 150, 0), 3));
    p.drawRoundedRect(rectBulle, rayon, rayon);

    const int fleche = 12;
    QPolygon arrow;
    arrow << QPoint(cx - fleche, bulleY) << QPoint(cx + fleche, bulleY) << QPoint(cx, bulleY - fleche);
    p.setBrush(QColor(255, 230, 60, 230));
    p.setPen(Qt::NoPen);
    p.drawPolygon(arrow);

    p.setPen(QColor(80, 40, 0));
    int textY = bulleY + 16;
    for (const QString& ligne : lignes) {
        QRect lr(bulleX + 16, textY, bulleLargeur - 32, lineH);
        p.drawText(lr, Qt::AlignHCenter | Qt::AlignVCenter, ligne);
        textY += lineH;
    }

    if (!enFermeture && tempsOuverture.elapsed() > DUREE_POP_MS) {
        QFont petitFont;
        petitFont.setPixelSize(int(height() * 0.022f));
        petitFont.setItalic(true);
        p.setFont(petitFont);
        p.setPen(QColor(200, 200, 200, int(160 * alpha)));
        p.drawText(
            QRect(0, height() - int(height() * 0.08f), width(), int(height() * 0.06f)),
            Qt::AlignHCenter | Qt::AlignVCenter,
            "[ cliquer ou appuyer sur une touche pour fermer ]"
        );
    }
}

void EasterEggDialog::mousePressEvent(QMouseEvent*)
{
    fermer();
}

void EasterEggDialog::keyPressEvent(QKeyEvent*)
{
    fermer();
}

void EasterEggDialog::fermer()
{
    if (enFermeture) {
        return;
    }

    enFermeture = true;
    tempsFermeture = 0;

    QElapsedTimer* t = new QElapsedTimer();
    t->start();

    connect(&timerAnim, &QTimer::timeout, this, [this, t]() {
        tempsFermeture = t->elapsed();
        if (tempsFermeture >= DUREE_FERMETURE_MS) {
            delete t;
            close();
        }
    });
}