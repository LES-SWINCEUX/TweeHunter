#!/usr/bin/env python3
"""
Affiche et réinitialise la configuration de TweeHunter (Windows registry).
HKCU\Software\TweeHunter\ConfigPartie\config
"""

import sys

try:
    import winreg
except ImportError:
    print("Ce script est pour Windows seulement.")
    sys.exit(1)

REG_PATH = r"Software\TweeHunter\ConfigPartie\config"

# Valeurs par défaut (miroir de GestionnaireConfiguration::charger)
DEFAUTS = {
    "arme":       1,
    "modeJeu":    1,   # ModeJeu::MOINS_18
    "difficulte": 0,   # DifficultePartie::NORMAL
    "manette":    0,   # TypeManette::STANDARD
    "nomJoueur":  "",
    "powerUp":    0,   # PowerUpType::GRENADE
}

LABELS = {
    "modeJeu":    {0: "PLUS_18",        1: "MOINS_18"},
    "difficulte": {0: "NORMAL",         1: "RNG",         2: "CHAOS"},
    "manette":    {0: "STANDARD",       1: "CUSTOM",      2: "CLAVIER_SOURIS"},
    "powerUp":    {0: "GRENADE",        1: "ZAP",         2: "MITRAILLETTE", 3: "TACTICAL_NUKE"},
}

def lire_config():
    config = {}
    try:
        cle = winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH)
    except FileNotFoundError:
        return config

    i = 0
    while True:
        try:
            nom, valeur, _ = winreg.EnumValue(cle, i)
            config[nom] = valeur
            i += 1
        except OSError:
            break

    winreg.CloseKey(cle)
    return config

def afficher_config(config):
    print(f"\n{'Paramètre':<15} {'Valeur actuelle':<25} {'Par défaut'}")
    print("-" * 60)

    for cle, defaut in DEFAUTS.items():
        valeur = config.get(cle, "(absent)")

        if cle in LABELS and isinstance(valeur, int):
            valeur_str = f"{LABELS[cle].get(valeur, valeur)} ({valeur})"
        else:
            valeur_str = str(valeur) if valeur != "" else "(vide)"

        if cle in LABELS and isinstance(defaut, int):
            defaut_str = f"{LABELS[cle].get(defaut, defaut)} ({defaut})"
        else:
            defaut_str = str(defaut) if defaut != "" else "(vide)"

        print(f"{cle:<15} {valeur_str:<25} {defaut_str}")

def effacer_config():
    try:
        cle = winreg.OpenKey(
            winreg.HKEY_CURRENT_USER, REG_PATH,
            access=winreg.KEY_ALL_ACCESS
        )
    except FileNotFoundError:
        print("Rien à effacer.")
        return

    # Supprime toutes les valeurs
    while True:
        try:
            nom, _, _ = winreg.EnumValue(cle, 0)
            winreg.DeleteValue(cle, nom)
        except OSError:
            break

    winreg.CloseKey(cle)

def main():
    config = lire_config()

    if not config:
        print("Aucune configuration trouvée — déjà aux valeurs par défaut.")
        return

    afficher_config(config)

    print()
    confirmation = input("Réinitialiser la configuration ? [o/N] ").strip().lower()
    if confirmation == "o":
        effacer_config()
        print("✓ Configuration réinitialisée.")
    else:
        print("Annulé.")

if __name__ == "__main__":
    main()
