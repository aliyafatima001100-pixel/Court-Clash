# 🎾 Court Clash

A feature-rich 2D tennis simulation game built in C++ using the Raylib library.
Choose a real-world tennis pro, outsmart a memory mini-game, and compete in a 
full match with physics, AI, video cutscenes, and a smash mini-game.

#🎥 Video Showcase

<p align="center">
  <img src="assets/demo/gameplay.gif" width="700"/>
</p>

---

## ✨ Features

- 🎯 **Player Selection** — Choose from Jannik Sinner, Carlos Alcaraz, or Daniil Medvedev
- 🧠 **Memory Mini-Game** — Your completion speed determines opponent
- 🎾 **Full Tennis Scoring** — Points, games, sets, deuce, and advantage logic
- 🎬 **Video Cutscenes** — Real match clips on set wins and championship wins
- ⚡ **Smash Mini-Game** — Press SPACE ×3 during smash opportunities
- 🤖 **CPU AI** — Baseline, net approach, volley, drop shot, and retreat tactics
- 🎨 **Animated UI** — Transitions, hover effects, and a serve power bar

---

## 🎮 How to Play

1. Press SPACE on the main menu to start
2. Select your player using Arrow Keys, confirm with SPACE
3. Complete the memory card game — faster = easier opponent
4. Read the rules screen, then press SPACE to begin the match
5. Use keyboard controls to move and hit the ball
6. First to win 2 sets wins the match

---

## 🕹️ Controls

| Key | Action |
|---|---|
| W / A / S / D | Move player |
| Arrow Keys | Move player (alternate) |
| SPACE | Serve / Hit ball |
| SPACE + W | Lob shot |
| SPACE + S | Drop shot |
| SPACE + A or D | Directional shot |
| SPACE ×3 | Smash (during smash event) |

---

## 🧠 OOP Concepts Demonstrated

| Concept | Where Used |
|---|---|
| Abstract base class | `Scene` with pure virtual `update()` and `draw()` |
| Inheritance | All scenes inherit from `Scene` |
| Polymorphism | `sceneStack` holds any `Scene*` at runtime |
| Encapsulation | `Card` class with private members and getters/setters |
| Operator overloading | `Card::operator==` for memory match logic |
| Templates | `SelectionSort<T>` applied to card deck |
| Exception handling | `FileLoadException` thrown on failed asset load |
| Recursion | `clearScenesRecursive()` clears the scene stack |
| STL stack | Scene manager built on `std::stack<Scene*>` |

---

## 🎬 Video Cutscene Pipeline

Raw `.mp4` footage is converted using FFmpeg, then decoded at runtime:

```bash
ffmpeg -i input.mp4 -vcodec mpeg1video -acodec mp2 -b:v 1500k output.mpg
```

The `.mpg` file is decoded frame-by-frame using `pl_mpeg`, uploaded to a GPU
texture via Raylib, and synced with a `.mp3` audio stream — all in real time.

---

## 🛠️ Requirements

- C++17 or later
- [Raylib](https://www.raylib.com/) installed and linked at C:/raylib
- `pl_mpeg.h` included (single-header, already in repo)

---

## ⚙️ Build & Run

### Use Make

```bash
make
make run
```

---

## 📁 Project Structure

```text
Court-Clash/
├── src/
│   └── main.cpp
├── assets/
│   ├── images/
│   ├── videos/
│   └── demo/
│       └── gameplay.gif
├── pl_mpeg.h
├── Makefile
├── README.md
├── raylib.h
└── raymath.h
```

---

## 🎥 Demo

[▶ Watch Gameplay on Google Drive](https://drive.google.com/file/d/1NvI5YGaPl_L31pG5x8NWtRdjum1sgGnP/view?usp=sharing)

---


