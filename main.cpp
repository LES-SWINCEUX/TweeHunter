#include "main_window.h"

#include <QFontDatabase>
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    locale::global(locale{ "" });
    QApplication a(argc, argv);
    qRegisterMetaType<ConfigurationPartie>("ConfigurationPartie");
    MainWindow w;

    int id = QFontDatabase::addApplicationFont(QDir::currentPath() + "/fonts/PressStart2P-Regular.ttf");
    if (id == -1) {
        std::cout << "Impossible de charger le style d'écriture:" << QDir::currentPath().toStdString() << "/fonts/PressStart2P-Regular.ttf" << std::endl;
    }

    id = QFontDatabase::addApplicationFont(QDir::currentPath() + "/fonts/LuckiestGuy-Regular.ttf");
    if (id == -1) {
        std::cout << "Impossible de charger le style d'écriture:" << QDir::currentPath().toStdString() << "/fonts/LuckiestGuy-Regular.ttf" << std::endl;
    }

    QCoreApplication::setOrganizationName("TweeHunter");
    QCoreApplication::setApplicationName("TweeHunter");


    w.setWindowTitle("TweeHunter");
    w.showMaximized();
    //w.showFullScreen();

    return a.exec();
}
