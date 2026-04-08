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

## 🧹 En cas de problème de compilation

Si le projet ne compile pas correctement, suis cette procédure avant d’aller plus loin.

### 1. Nettoyer la solution
Exécute le script `clean.py` pour supprimer les fichiers générés et repartir sur une base propre.

```bash
python clean.py
```

Ce script permet notamment de nettoyer les fichiers et dossiers temporaires liés à Visual Studio, qmake et aux builds précédents.

### 2. Régénérer le projet
Après le nettoyage :

1. Rouvre le projet avec **Visual Studio**
2. Recharge `TweeHunter.pro`
3. **Extensions → Qt VS Tools → Open Qt Project File (.pro)**
4. Recompile la solution

### 3. Installer SDL3 si nécessaire
Si la compilation échoue encore et que le problème semble venir de **SDL3** (bibliothèque manquante, lien impossible, include introuvable, etc.), exécute le script suivant :

```bash
python install_sdl3.py
```

Ce script sert à installer/configurer **SDL3** pour le projet.

### 4. Recompiler
Une fois SDL3 installé, rebuild la solution dans Visual Studio.

**En résumé :**  
> `clean.py` sert à repartir d’un environnement propre  
> `install_sdl3.py` sert à installer SDL3 si le projet ne compile pas à cause de cette dépendance

---

## 🧪 Notes importantes

- Toute modification dans :
  - `SOURCES`
  - `HEADERS`
  - `FORMS`
  - `RESOURCES`  
  nécessite de **relancer qmake** (`Extensions → Qt VS Tools → Open Qt Project File (.pro)`).
- Les dossiers `debug/`, `release/` et `.vs/` ne doivent **pas** être commités.
- Les fichiers générés par Qt (`moc_*.cpp`, `ui_*.h`, etc.) sont automatiquement recréés.

---

## 📌 Objectifs du projet (roadmap)

- [x] Mise en place d’une scène de jeu
- [x] Gestion de la folle manette de jeu
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
