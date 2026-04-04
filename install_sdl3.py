#!/usr/bin/env python3

import os
import sys
import platform
import subprocess
import urllib.request
import zipfile
import tarfile

SDL3_VERSION = "3.4.0"
SDL3_BASE_URL = f"https://github.com/libsdl-org/SDL/releases/download/release-{SDL3_VERSION}"
DEST_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "SDL3")
SDL3_DIR = os.path.join(DEST_DIR, f"SDL3-{SDL3_VERSION}")
SENTINEL = os.path.join(SDL3_DIR, "include", "SDL3", "SDL.h")


def log(msg):
    print(f"[SDL3] {msg}")


def download(url, dest):
    log(f"Téléchargement de {url} ...")
    def progress(count, block_size, total):
        if total > 0:
            pct = min(count * block_size * 100 // total, 100)
            print(f"\r  {pct}%", end="", flush=True)
    urllib.request.urlretrieve(url, dest, reporthook=progress)
    print()  # newline après la barre de progression


def install_windows():
    zip_url = f"{SDL3_BASE_URL}/SDL3-devel-{SDL3_VERSION}-VC.zip"
    zip_path = os.path.join(DEST_DIR, "SDL3.zip")

    os.makedirs(DEST_DIR, exist_ok=True)
    download(zip_url, zip_path)

    log("Extraction...")
    with zipfile.ZipFile(zip_path, "r") as z:
        z.extractall(DEST_DIR)

    os.remove(zip_path)
    log(f"SDL3 installé dans : {SDL3_DIR}")


def install_linux():
    tar_url = f"{SDL3_BASE_URL}/SDL3-{SDL3_VERSION}.tar.gz"
    tar_path = os.path.join(DEST_DIR, "SDL3.tar.gz")

    log("Sur Linux, SDL3 doit être compilé depuis les sources.")
    log("Il est recommandé d'utiliser le gestionnaire de paquets :")
    log("  Ubuntu/Debian : sudo apt install libsdl3-dev")
    log("  Arch          : sudo pacman -S sdl3")
    log("  Fedora        : sudo dnf install SDL3-devel")
    print()

    choix = input("Voulez-vous quand même télécharger les sources ? (o/N) : ").strip().lower()
    if choix != "o":
        log("Installation annulée.")
        sys.exit(0)

    os.makedirs(SDL3_DIR, exist_ok=True)
    download(tar_url, tar_path)

    log("Extraction...")
    with tarfile.open(tar_path, "r:gz") as t:
        members = [m for m in t.getmembers()]
        # strip-components=1 : retirer le dossier racine de l'archive
        for member in members:
            parts = member.name.split("/", 1)
            if len(parts) > 1:
                member.name = parts[1]
                t.extract(member, SDL3_DIR)

    os.remove(tar_path)
    log(f"Sources extraites dans : {SDL3_DIR}")
    log("Pour compiler : cd SDL3/SDL3-3.4.0 && cmake -B build && cmake --build build")


def install_macos():
    log("Sur macOS, il est recommandé d'utiliser Homebrew :")
    log("  brew install sdl3")
    print()
    input("Appuyez sur Entrée pour continuer ou Ctrl+C pour annuler... ")
    subprocess.run(["brew", "install", "sdl3"], check=True)


def main():
    if os.path.isfile(SENTINEL):
        log(f"SDL3 {SDL3_VERSION} est déjà installé. Rien à faire.")
        sys.exit(0)

    system = platform.system()
    log(f"Plateforme détectée : {system}")

    if system == "Windows":
        install_windows()
    elif system == "Linux":
        install_linux()
    elif system == "Darwin":
        install_macos()
    else:
        log(f"Plateforme non supportée : {system}")
        sys.exit(1)

    if os.path.isfile(SENTINEL):
        log("Installation vérifiée avec succès ✓")
    else:
        log("Avertissement : SDL.h introuvable après installation, vérifiez manuellement.")


if __name__ == "__main__":
    main()