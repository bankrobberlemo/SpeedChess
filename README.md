# Speed Chess

Project for the **Computer Graphics** course as part of the Master 'Angewandte Informatik' at the Hochschule Fulda.

Built on top of the OpenGL renderer from the lecture, this is a local 2-player real-time chess game using SDL3 with OpenGL 4.6 for 2D rendering with orthographic projection.

## How it works

Speed Chess is a real-time twist on classic chess. Both players move simultaneously — there are no turns. Each player has an action bar with 4 charges that recharges over time. Every move costs one charge.

The goal is to capture the opponent's king (not checkmate, but actually taking it off the board). This means the king is allowed to move into check.

Power-ups can optionally be enabled. They spawn randomly on the board and include Teleport, Bomb, Freeze and Promotion.

## Controls

| Action         | Player 1 (White)     | Player 2 (Black)     |
|----------------|----------------------|----------------------|
| Move cursor    | Arrow keys           | W / A / S / D        |
| Select / Confirm | Enter              | Space                |
| Cancel         | Delete               | Tab                  |

- **ESC** — Pause
- **F11** — Fullscreen

## Menu

The main menu lets you pick the speed (Fast / Normal / Slow) and toggle power-ups on or off. Volume settings for music and sound effects can be adjusted under Settings.

## Build

Requirements: CMake 3.22+, clang++-20 with libc++

```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_COMPILER=/usr/bin/clang-20 \
      -DCMAKE_CXX_COMPILER=/usr/bin/clang++-20 \
      -S . -B build

cmake --build build
```

The executable will be at `bin/opengl-renderer`.
