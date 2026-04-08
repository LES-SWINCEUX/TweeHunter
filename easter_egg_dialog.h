#ifndef EASTER_EGG_DIALOG_H
#define EASTER_EGG_DIALOG_H

#include <QDialog>
#include <QPainter>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFont>
#include <QFontMetrics>
#include <QtMath>
#include <QDir>
#include "sprite_manager.h"

class EasterEggDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EasterEggDialog(int indexCanette, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void fermer();

    static const QStringList& messages();

    int indexCanette = 0;

    QSharedPointer<QPixmap> spriteCanette;

    QTimer timerAnim;
    QElapsedTimer tempsOuverture;

    bool enFermeture = false;
    qint64 tempsFermeture = 0;

    const int DUREE_POP_MS = 350;
    const int DUREE_FERMETURE_MS = 250;
};

#endif
