#!/usr/bin/env python3
from pathlib import Path
import shutil
import sys

DIR_NAMES = {"debug", "release", "SDL3"}

EXACT_FILE_NAMES = {
    ".qmake.stash",
}

FILE_SUFFIXES = {
    ".qtvscr",
    ".vcxproj",
    ".vcxproj.filters",
    ".vcxproj.user",
}


def should_delete_file(path: Path) -> bool:
    if path.name in EXACT_FILE_NAMES:
        return True

    for suffix in FILE_SUFFIXES:
        if path.name.endswith(suffix):
            return True

    return False


def delete_target(path: Path) -> None:
    try:
        if path.is_dir():
            shutil.rmtree(path)
            print(f"[SUPPRIMÉ DOSSIER] {path}")
        elif path.is_file():
            path.chmod(0o777)  # aide si le fichier est en lecture seule
            path.unlink()
            print(f"[SUPPRIMÉ FICHIER] {path}")
    except Exception as e:
        print(f"[ERREUR] {path} -> {e}")


def main() -> int:
    root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path.cwd()

    if not root.exists():
        print(f"Le chemin n'existe pas : {root}")
        return 1

    print(f"Nettoyage dans : {root}")

    to_delete = []

    for path in root.rglob("*"):
        if path.is_dir() and path.name in DIR_NAMES:
            to_delete.append(path)
        elif path.is_file() and should_delete_file(path):
            to_delete.append(path)

    to_delete.sort(key=lambda p: len(p.parts), reverse=True)

    if not to_delete:
        print("Aucun élément à supprimer.")
        return 0

    for path in to_delete:
        delete_target(path)

    print("Nettoyage terminé.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())