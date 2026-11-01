# Pacman — C++17 / SFML 2.6

A Pac-Man inspired arcade game built with C++17 and SFML 2.6.

## Features

- Arcade menu with keyboard-driven settings.
- Solo mode and local two-player co-op.
- Six selectable maze variants; the campaign advances through six levels.
- Six visual themes: Classic, Neon, Fire, Ice, Forest and Mono.
- Six Pac-Man color choices.
- Four ghosts, pellets, power pellets, score, lives and frightened mode.
- Pause, game-over and campaign victory screens.
- CMake build and GitHub Actions build workflow.

## Controls

### Menu
- Up / Down: select a setting
- Left / Right: change the selected setting
- Enter: start

### Gameplay
- Player 1: W A S D
- Player 2: arrow keys (only active in local 2-player mode)
- P / Escape: pause or resume
- Q: quit

## Requirements

- Windows with Visual Studio 2022 C++ tools, or another C++17 toolchain
- CMake 3.20+
- SFML 2.6.x (Graphics, Window and System)

## Build

Configure with the SFML package path available on your machine:

```powershell
cmake -S . -B build -DSFML_DIR="C:/path/to/SFML/lib/cmake/SFML"
cmake --build build --config Release
```

Run `build/Release/pacman.exe`. Make sure the SFML 2.6 runtime DLLs are next to the executable or available on PATH.

## Notes

- Multiplayer is local co-op on one keyboard, not online networking.
- The six maze selections are generated from the base maze using mirrored layouts and pellet variations.
- The campaign has six stages and cycles through the selected maze variants.
- The game draws its artwork with SFML shapes; no sprite pack is required.
