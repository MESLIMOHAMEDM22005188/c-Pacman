# Pacman — C++ / SFML

A complete Pac-Man style arcade game built with C++17 and SFML 2.6.

## Features

- Classic maze gameplay with pellets, power pellets and four ghosts.
- Ghost AI with chase, scatter, frightened and returning-to-home states.
- Score, lives, levels and increasing difficulty.
- Start, pause, game-over and victory screens.
- Keyboard controls, clean separation between game logic and rendering.
- CMake build configuration and Windows-friendly setup instructions.

## Requirements

- C++17 compiler
- CMake 3.20+
- SFML 2.6 (graphics, window and system)

## Build

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run the generated `pacman` executable from the build directory (on Windows, typically `build/Release/pacman.exe` or `build/pacman.exe`).

## Controls

- Arrow keys / WASD: move
- Enter: start or restart
- P / Escape: pause
- Q: quit

## Project layout

```text
include/       Public game headers
src/           C++ implementation
assets/        Optional resources
CMakeLists.txt Build configuration
```

The game uses SFML shapes and text for its visuals, so no external sprite pack is required.