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

    id = QFontDatabase::addApplicationFont(QDir::currentPath() + "/fonts/LuckiestGuy-Regular.ttf");

    QCoreApplication::setOrganizationName("TweeHunter");
    QCoreApplication::setApplicationName("TweeHunter");


    w.setWindowTitle("TweeHunter");
    w.showMaximized();
    //w.showFullScreen();

    return a.exec();
}
