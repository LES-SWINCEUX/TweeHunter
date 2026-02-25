#include "Touches.h"

Touches::Touches()
{
    gamepad = nullptr;

    joystick = false;
    //Partie joystick de test
    //Initialisation du SDL ---
    if (SDL_Init(SDL_INIT_GAMEPAD) < 0)
    {
        qDebug() << "Erreur SDL:" << SDL_GetError();
        return;
    }

    //Détection de la manette ---
    int count = 1;
    SDL_JoystickID* ids = SDL_GetGamepads(&count); // Liste des gamepads
    qDebug() << "Nombre de manettes detectees:" << count;


    if (ids && count > 0)
    {
        // Ouvrir la première manette
        gamepad = SDL_OpenGamepad(ids[0]);
        if (gamepad) {
            cout << "Manette ouverte !" << endl;
            joystick = true;
        }else
            cout << "Erreur ouverture:" << SDL_GetError();

        SDL_free(ids); // libérer la mémoire retournée par SDL_GetGamepads
    
    }
    else {
        cout << "Aucune manette" << endl;

    }



}

Touches::~Touches() {
}

bool Touches::RTpressed() const {
    Sint16 value = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (value > 10000) {
		return true;
    }

	return false;
}

