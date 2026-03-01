# 🎮 TweeHunter

**TweeHunter** est un projet de jeu développé en **C++** avec **Qt Widgets**, utilisant **qmake** comme système de build et **Visual Studio (MSVC)** comme environnement de développement.

Le projet est actuellement en phase de développement et sert de base pour explorer :
- la structure d’un jeu 2D avec Qt
- la gestion de scènes, entrées clavier et logique de jeu
- l’intégration propre de Qt avec Visual Studio

---

## 🛠️ Technologies utilisées

- **Langage** : C++17  
- **Framework** : Qt (Qt Widgets)
- **Build system** : qmake
- **IDE** : Visual Studio 2022
- **Compilateur** : MSVC (x64)

---

## 📁 Structure du projet

> Le fichier **`TweeHunter.pro`** est la **source de vérité** du projet.  
> Les fichiers Visual Studio (`.sln`, `.vcxproj`, etc.) sont générés automatiquement.

---

## 🚀 Prérequis

Avant de compiler le projet, assure-toi d’avoir :

- **Qt 6.x (MSVC 64-bit)** installé  
  - ex : `C:\Qt\6.6.2\msvc2022_64`
- **Visual Studio 2022**
  - avec le workload **Développement Desktop C++**
- **Qt Visual Studio Tools** (extension officielle)

---

## ▶️ Ouvrir et compiler le projet

### Depuis Visual Studio
1. Ouvre Visual Studio
2. **Extensions → Qt VS Tools → Open Qt Project File (.pro)**
3. Sélectionne `TweeHunter.pro`
4. Clique droit sur le projet → **Qt → Run qmake**
5. **Build → Build Solution** ou **F5**

### Configurations disponibles
- `Debug | x64`
- `Release | x64`

---

## 🧪 Notes importantes

- Toute modification dans :
  - `SOURCES`
  - `HEADERS`
  - `FORMS`
  - `RESOURCES`  
  nécessite de **relancer qmake** (`Qt → Run qmake`).
- Les dossiers `debug/`, `release/` et `.vs/` ne doivent **pas** être commités.
- Les fichiers générés par Qt (`moc_*.cpp`, `ui_*.h`, etc.) sont automatiquement recréés.

---

## 📌 Objectifs du projet (roadmap)

- [x] Mise en place d’une scène de jeu
- [ ] Gestion de la folle manette de jeu
- [x] Gestion des entrées clavier
- [x] Ajout d’un joueur et d’ennemis
- [x] Collisions et logique de jeu
- [x] Système de score / états du jeu

---

## 👤 Auteur

Développé par les GOAT **Antoine Blais**, **Michael Normand** et **Philip Pigeon**

---

> Ce projet est volontairement simple et évolutif afin de servir de base d’apprentissage
> pour le développement de jeux avec Qt et C++.
