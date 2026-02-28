#include "main_window.h"

#include <QFontDatabase>
#include <QApplication>

int main(int argc, char *argv[])
{
    locale::global(locale{ "" });
    QApplication a(argc, argv);
    MainWindow w;

    int id = QFontDatabase::addApplicationFont(QDir::currentPath() + "/fonts/PressStart2P-Regular.ttf");
    if (id == -1) {
        std::cout << "Impossible de charger le style d'écriture:" << QDir::currentPath().toStdString() << "/fonts/PressStart2P-Regular.ttf" << std::endl;
    }

    w.setWindowTitle("TweeHunter");
    w.showMaximized();
    //w.showFullScreen();

    return a.exec();
}
