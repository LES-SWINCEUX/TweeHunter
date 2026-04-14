#!/usr/bin/env python3
"""
Affiche et efface les scores de TweeHunter (Windows registry).
HKCU\Software\TweeHunter\Scores\scores
"""

import sys

try:
    import winreg
except ImportError:
    print("Ce script est pour Windows seulement.")
    sys.exit(1)

REG_PATH = r"Software\TweeHunter\Scores\scores"

def lire_scores():
    scores = []
    try:
        cle = winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH)
    except FileNotFoundError:
        return scores

    i = 0
    while True:
        try:
            sous_nom = winreg.EnumKey(cle, i)
            sous_cle = winreg.OpenKey(cle, sous_nom)
            nom   = winreg.QueryValueEx(sous_cle, "nom")[0]
            score = winreg.QueryValueEx(sous_cle, "score")[0]
            scores.append((sous_nom, nom, score))
            winreg.CloseKey(sous_cle)
            i += 1
        except OSError:
            break

    winreg.CloseKey(cle)
    scores.sort(key=lambda x: int(x[0]))
    return scores

def effacer_scores():
    try:
        cle = winreg.OpenKey(
            winreg.HKEY_CURRENT_USER, REG_PATH,
            access=winreg.KEY_ALL_ACCESS
        )
    except FileNotFoundError:
        print("Rien à effacer.")
        return

    while True:
        try:
            sous_nom = winreg.EnumKey(cle, 0)
            winreg.DeleteKey(cle, sous_nom)
        except OSError:
            break

    winreg.SetValueEx(cle, "size", 0, winreg.REG_QWORD, 0)
    winreg.CloseKey(cle)

def main():
    scores = lire_scores()

    if not scores:
        print("Aucun score trouvé — déjà propre.")
        return

    print(f"{'#':<4} {'Nom':<25} {'Score':>10}")
    print("-" * 42)
    for idx, nom, score in scores:
        print(f"{idx:<4} {nom:<25} {score:>10}")

    print()
    confirmation = input("Effacer tous les scores ? [o/N] ").strip().lower()
    if confirmation == "o":
        effacer_scores()
        print("✓ Scores effacés.")
    else:
        print("Annulé.")

if __name__ == "__main__":
    main()
