#include "ecran_parametres.h"

QSize EcranParametres::tailleSource(const QSharedPointer<QPixmap>& pixmap) const
{
    if (!pixmap || pixmap->isNull()) {
        return QSize();
    }
    return pixmap->size();
}

QRect EcranParametres::calculerRectTitre(const QSharedPointer<QPixmap>& pixmap, int centreX, int y, int hauteurCible, int largeurMax) const
{
    const QSize source = tailleSource(pixmap);
    if (!source.isValid() || hauteurCible <= 0 || largeurMax <= 0) {
        return QRect();
    }

    float echelle = float(hauteurCible) / float(source.height());
    int largeur = int(source.width() * echelle);
    int hauteur = int(source.height() * echelle);

    if (largeur > largeurMax) {
        echelle = float(largeurMax) / float(source.width());
        largeur = int(source.width() * echelle);
        hauteur = int(source.height() * echelle);
    }

    return QRect(centreX - largeur / 2, y, largeur, hauteur);
}

void EcranParametres::dessinerPixmap(QPainter& painter,
    const QSharedPointer<QPixmap>& pixmap,
    const QRect& destination) const
{
    if (!pixmap || pixmap->isNull() || !destination.isValid()) {
        return;
    }

    painter.drawPixmap(destination, *pixmap, pixmap->rect());
}

int EcranParametres::placerGrille(const std::vector<Bouton*>& boutons, int colonnes, int x, int y, int largeurDisponible, int hauteurBoutonCible, int espacementX, int espacementY)
{
    if (boutons.empty() || colonnes <= 0 || largeurDisponible <= 0) {
        return 0;
    }

    const QSize base = boutons.front()->tailleImage();
    if (!base.isValid()) {
        return 0;
    }

    const int celluleW = (largeurDisponible - (colonnes - 1) * espacementX) / colonnes;

    float echelle = std::min(float(celluleW) / float(base.width()),
        float(hauteurBoutonCible) / float(base.height()));
    echelle = std::clamp(echelle, 0.18f, 1.25f);

    for (Bouton* bouton : boutons) {
        if (bouton) {
            bouton->setEchelle(echelle);
        }
    }

    const int largeurBouton = boutons.front()->width();
    const int hauteurBouton = boutons.front()->height();
    const int nombreLignes = int((boutons.size() + colonnes - 1) / colonnes);

    for (int ligne = 0; ligne < nombreLignes; ++ligne) {
        const int debut = ligne * colonnes;
        const int nombreSurLigne = std::min(colonnes, int(boutons.size()) - debut);

        for (int i = 0; i < nombreSurLigne; ++i) {
            Bouton* bouton = boutons[debut + i];
            if (!bouton) {
                continue;
            }

            bouton->move(
                x + i * (largeurBouton + espacementX),
                y + ligne * (hauteurBouton + espacementY)
            );
        }
    }

    return nombreLignes * hauteurBouton + (nombreLignes - 1) * espacementY;
}

void EcranParametres::placerElements()
{
    const int W = width();
    const int H = height();
    if (W <= 0 || H <= 0) {
        return;
    }

    const float echelleH = std::clamp(float(H) / 1080.0f, 0.72f, 1.12f);
    const float echelleW = std::clamp(float(W) / 1920.0f, 0.80f, 1.20f);
    const float echelle = std::min(echelleH, echelleW);

    const int centreX = W / 2;
    const int margeX = std::max(16, int(W * 0.02f));
    const int largeurContenu = W - 2 * margeX;

    const int espacementX = std::max(10, int(14 * echelleW));
    const int espacementY = std::max(8, int(10 * echelle));
    const int espacementTitre = std::max(4, int(5 * echelle));
    const int espacementSection = std::max(10, int(12 * echelle));
    const int ecartColonnes = std::max(12, int(16 * echelleW));

    int y = std::max(8, int(10 * echelle));

    auto estimerLargeurGrille = [&](Bouton* boutonRef, int colonnes, int largeurMax, int hauteurBoutonCible, int spacingX) -> int
    {
        if (!boutonRef || colonnes <= 0 || largeurMax <= 0) {
            return 0;
        }

        const QSize base = boutonRef->tailleImage();
        if (!base.isValid()) {
            return 0;
        }

        const int celluleW = (largeurMax - (colonnes - 1) * spacingX) / colonnes;

        float scale = std::min(float(celluleW) / float(base.width()),
            float(hauteurBoutonCible) / float(base.height()));
        scale = std::clamp(scale, 0.18f, 1.25f);

        const int largeurBouton = int(base.width() * scale);
        return colonnes * largeurBouton + (colonnes - 1) * spacingX;
    };

    rectTitrePrincipal = calculerRectTitre(titrePrincipalImg, centreX, y, int(118 * echelle), int(largeurContenu * 0.52f));

    y = rectTitrePrincipal.bottom() + 1 + espacementSection;

    const int hauteurCartesHaut = int(150 * echelle);

    const int largeurArmes = estimerLargeurGrille(boutonCarabine, 3, int(largeurContenu * 0.48f), hauteurCartesHaut, espacementX);

    const int largeurPowerUps = estimerLargeurGrille(boutonGrenade, 2, int(largeurContenu * 0.28f), hauteurCartesHaut, espacementX);

    const int largeurTop = largeurArmes + ecartColonnes + largeurPowerUps;
    const int xTop = centreX - largeurTop / 2;

    const int xArmes = xTop;
    const int xPowerUps = xArmes + largeurArmes + ecartColonnes;

    appliquerDisponibiliteManettes();

    const int hauteurBanniereHaut = int(38 * echelle);

    rectTitreArmes = calculerRectTitre(titreChoixArmeImg, xArmes + largeurArmes / 2, y, hauteurBanniereHaut, int(largeurArmes * 0.52f));

    rectTitrePowerUps = calculerRectTitre(titrePowerUpsImg, xPowerUps + largeurPowerUps / 2, y, hauteurBanniereHaut, int(largeurPowerUps * 1.18f));

    const int yTop = std::max(rectTitreArmes.bottom(), rectTitrePowerUps.bottom()) + 1 + espacementTitre;

    const int hauteurArmes = placerGrille({ boutonCarabine, boutonShotgun, boutonBombardier, boutonGrpc, boutonTarte, boutonSwince }, 3, xArmes, yTop, largeurArmes, hauteurCartesHaut, espacementX, std::max(6, espacementY / 2));

    const int hauteurPowerUps = placerGrille({ boutonGrenade, boutonZap, boutonMitraillette, boutonTacticalNuke }, 2, xPowerUps, yTop, largeurPowerUps, hauteurCartesHaut, espacementX, std::max(6, espacementY / 2));

    y = yTop + std::max(hauteurArmes, hauteurPowerUps) + espacementSection;

    const int hauteurBoutonsMilieu = std::clamp(int(70 * echelle), 56, 82);

    const int largeurMode18 = std::clamp(int(165 * echelleW), 130, 220);
    const int largeurModeNormal = std::clamp(int(210 * echelleW), 160, 280);

    const int largeurDiff = std::clamp(int(180 * echelleW), 145, 240);
    const int espacementDiff = std::max(8, int(10 * echelleW));

    const int largeurModeJeu = largeurMode18 + espacementX + largeurModeNormal;
    const int largeurDifficulte = largeurDiff * 3 + espacementDiff * 2;

    const int largeurMilieu = largeurModeJeu + ecartColonnes + largeurDifficulte;
    const int xMilieu = centreX - largeurMilieu / 2;

    const int xModeJeu = xMilieu;
    const int xDifficulte = xModeJeu + largeurModeJeu + ecartColonnes;

    rectTitreModeJeu = calculerRectTitre(titreModeJeuImg, xModeJeu + largeurModeJeu / 2, y, int(36 * echelle), int(largeurModeJeu * 0.82f));

    rectTitreDifficulte = calculerRectTitre(titreDifficulteImg, xDifficulte + largeurDifficulte / 2, y, int(36 * echelle), int(largeurDifficulte * 0.62f));

    const int yMilieu = std::max(rectTitreModeJeu.bottom(), rectTitreDifficulte.bottom()) + 1 + espacementTitre;

    if (boutonMode18) {
        boutonMode18->setGeometry(xModeJeu, yMilieu, largeurMode18, hauteurBoutonsMilieu);
    }
    if (boutonModeNormal) {
        boutonModeNormal->setGeometry(xModeJeu + largeurMode18 + espacementX, yMilieu, largeurModeNormal, hauteurBoutonsMilieu);
    }

    if (boutonDifficulteNormal) {
        boutonDifficulteNormal->setGeometry(xDifficulte, yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }
    if (boutonDifficulteRng) {
        boutonDifficulteRng->setGeometry(xDifficulte + largeurDiff + espacementDiff, yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }
    if (boutonDifficulteChaos) {
        boutonDifficulteChaos->setGeometry(xDifficulte + 2 * (largeurDiff + espacementDiff), yMilieu, largeurDiff, hauteurBoutonsMilieu);
    }

    y = yMilieu + hauteurBoutonsMilieu + espacementSection;

    const int hauteurManette = int(145 * echelle);

    std::vector<Bouton*> boutonsManetteVisibles;
    if (boutonManetteStandard && boutonManetteStandard->isVisible()) {
        boutonsManetteVisibles.push_back(boutonManetteStandard);
    }
    if (boutonManetteCustom && boutonManetteCustom->isVisible()) {
        boutonsManetteVisibles.push_back(boutonManetteCustom);
    }
    if (boutonClavierSouris && boutonClavierSouris->isVisible()) {
        boutonsManetteVisibles.push_back(boutonClavierSouris);
    }

    int largeurManette = 0;
    int xManette = centreX;

    if (!boutonsManetteVisibles.empty()) {
        largeurManette = estimerLargeurGrille(boutonsManetteVisibles.front(), int(boutonsManetteVisibles.size()), int(largeurContenu * 0.44f), hauteurManette, espacementX);

        xManette = centreX - largeurManette / 2;
    }

    const int largeurBanniereManette = std::clamp(std::max(int(largeurManette * 0.90f), int(largeurContenu * 0.22f)), 260, 520);

    rectTitreChoixManette = calculerRectTitre(
        titreChoixManetteImg,
        centreX,
        y,
        int(38 * echelle),
        largeurBanniereManette
    );

    y = rectTitreChoixManette.bottom() + 1 + espacementTitre;

    if (!boutonsManetteVisibles.empty()) {
        y += placerGrille(boutonsManetteVisibles, int(boutonsManetteVisibles.size()), xManette, y, largeurManette, hauteurManette, espacementX, espacementY) + espacementSection;
    }

    const int largeurBlocNom = std::clamp(int(largeurContenu * 0.18f), 280, 380);

    rectTitreNom = calculerRectTitre(titreNomImg, centreX, y, int(38 * echelle), int(largeurBlocNom * 1.18f));

    y = rectTitreNom.bottom() + 1 + espacementTitre;

    rectFondNom = calculerRectTitre(fondNomImg, centreX, y, int(56 * echelle), largeurBlocNom);

    if (champNom) {
        QFont f = policeTitre;
        f.setPixelSize(std::max(12, int(rectFondNom.height() * 0.48f)));
        champNom->setFont(f);
        champNom->setGeometry(rectFondNom.adjusted(20, 8, -50, -8));
    }

    y = rectFondNom.bottom() + 1 + espacementSection;

    if (boutonRetour && boutonCommencer) {
        const QSize baseRetour = boutonRetour->tailleImage();
        const QSize baseJouer = boutonCommencer->tailleImage();

        if (baseRetour.isValid() && baseJouer.isValid()) {
            const int hauteurBoutonCible = std::clamp(int(68 * echelle), 48, 82);
            const int espaceEntreBoutons = std::max(14, int(18 * echelleW));

            float echelleRetour = float(hauteurBoutonCible) / float(baseRetour.height());
            float echelleJouer = float(hauteurBoutonCible) / float(baseJouer.height());

            echelleRetour = std::clamp(echelleRetour, 0.20f, 1.25f);
            echelleJouer = std::clamp(echelleJouer, 0.20f, 1.25f);

            boutonRetour->setEchelle(echelleRetour);
            boutonCommencer->setEchelle(echelleJouer);

            const int largeurTotale =
                boutonRetour->width() + espaceEntreBoutons + boutonCommencer->width();

            const int xDepart = centreX - largeurTotale / 2;

            boutonRetour->move(xDepart, y + (boutonCommencer->height() - boutonRetour->height()) / 2);

            boutonCommencer->move(xDepart + boutonRetour->width() + espaceEntreBoutons, y);
        }
    }

    centrerContenuVerticalement();

    aideFont_pixelSize = std::max(8, int(12 * std::clamp(float(H) / 1080.0f, 0.7f, 1.0f)));

    if (overlay) {
        overlay->setGeometry(rect());
        overlay->raise();
    }
}

void EcranParametres::centrerContenuVerticalement()
{
    int top = rectTitrePrincipal.top();

    int bottom = rectFondNom.bottom();
    if (boutonCommencer) {
        bottom = std::max(bottom, boutonCommencer->geometry().bottom());
    }
    if (boutonRetour) {
        bottom = std::max(bottom, boutonRetour->geometry().bottom());
    }

    const int hauteurContenu = bottom - top + 1;
    const int margeMin = 12;

    int decalageY = (height() - hauteurContenu) / 2 - top;

    if (top + decalageY < margeMin) {
        decalageY = margeMin - top;
    }

    if (decalageY == 0) {
        return;
    }

    rectTitrePrincipal.translate(0, decalageY);
    rectTitreArmes.translate(0, decalageY);
    rectTitrePowerUps.translate(0, decalageY);
    rectTitreModeJeu.translate(0, decalageY);
    rectTitreDifficulte.translate(0, decalageY);
    rectTitreChoixManette.translate(0, decalageY);
    rectTitreNom.translate(0, decalageY);
    rectFondNom.translate(0, decalageY);

    auto deplacerWidget = [decalageY](QWidget* w)
    {
        if (w) {
            w->move(w->x(), w->y() + decalageY);
        }
    };

    deplacerWidget(boutonCarabine);
    deplacerWidget(boutonShotgun);
    deplacerWidget(boutonBombardier);
    deplacerWidget(boutonGrpc);
    deplacerWidget(boutonTarte);
    deplacerWidget(boutonSwince);

    deplacerWidget(boutonGrenade);
    deplacerWidget(boutonZap);
    deplacerWidget(boutonMitraillette);
    deplacerWidget(boutonTacticalNuke);

    deplacerWidget(boutonMode18);
    deplacerWidget(boutonModeNormal);

    deplacerWidget(boutonDifficulteNormal);
    deplacerWidget(boutonDifficulteRng);
    deplacerWidget(boutonDifficulteChaos);

    deplacerWidget(boutonManetteStandard);
    deplacerWidget(boutonManetteCustom);
    deplacerWidget(boutonClavierSouris);

    deplacerWidget(champNom);
    deplacerWidget(boutonRetour);
    deplacerWidget(boutonCommencer);
}

void EcranParametres::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    bg.dessiner(painter);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    dessinerPixmap(painter, titrePrincipalImg, rectTitrePrincipal);
    dessinerPixmap(painter, titreChoixArmeImg, rectTitreArmes);
    dessinerPixmap(painter, titrePowerUpsImg, rectTitrePowerUps);
    dessinerPixmap(painter, titreModeJeuImg, rectTitreModeJeu);
    dessinerPixmap(painter, titreDifficulteImg, rectTitreDifficulte);
    dessinerPixmap(painter, titreChoixManetteImg, rectTitreChoixManette);
    dessinerPixmap(painter, titreNomImg, rectTitreNom);
    dessinerPixmap(painter, fondNomImg, rectFondNom);
}

void EcranParametres::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    bg.mettreAJour(size());

    placerElements();
}
