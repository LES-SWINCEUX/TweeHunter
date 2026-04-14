#include "main_window.h"

#ifdef Q_OS_WIN
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Résolution timer Windows à 1ms pour éviter les stagger partout
    timeBeginPeriod(1);
#endif

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
    //w.showMaximized();
    w.showFullScreen();

    int ret = a.exec();
    SDL_Quit();
#ifdef Q_OS_WIN
    timeEndPeriod(1);
#endif
    return ret;
}
