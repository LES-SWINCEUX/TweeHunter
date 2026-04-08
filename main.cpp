#include "main_window.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0) {
        qDebug() << "Erreur SDL:" << SDL_GetError();
    }

    std::locale::global(std::locale{ "" });
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

    int ret = a.exec();
    SDL_Quit();
    return ret;
}
