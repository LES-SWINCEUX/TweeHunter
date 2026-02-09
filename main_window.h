#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "menu.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void afficherMenuPrincipal();

    void afficherEcranJeu();

private:
    MenuPrincipal* menuPrincipal = nullptr;
};
#endif
