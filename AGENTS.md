# Repository Guidelines

## Project Structure & Module Organization
- Root CMake project (`CMakeLists.txt`) builds the `path_to_home` executable from `src/`.
- Source and headers live in `src/` (`main.cpp`, `Game.cpp`, `Meteor.cpp`, `Fuel.cpp`, `Hud.cpp`, `ParticleSystem.cpp`, etc.).
- Game assets reside in `resources/` (`resources/images/`, `resources/audio/`, `resources/fonts/`); the post-build step copies the whole `resources/` tree next to the binary.
- Default build output: `cmake-build-debug/bin/path_to_home` (adjust the build directory name as needed).

## Build, Test, and Development Commands
- Configure + build (debug): `cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug`
- Run from build dir: `./cmake-build-debug/bin/path_to_home`
- Use system SFML (default): `cmake -S . -B cmake-build-debug -DUSE_SYSTEM_SFML=ON`; to fetch vendored SFML, set `OFF`.
- Release-style build: `cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release`
- No automated tests exist; rely on a quick smoke run after each change.

## Coding Style & Naming Conventions
- Formatting: `clang-format` with `.clang-format` (LLVM base, 4-space indent, no tabs, 100-column limit, attached braces). Run it before committing.
- C++: classes/types in `PascalCase`; functions/methods in `camelCase`; member variables currently lean `camelCase` (`musicEnabled`, `progressStart`); constants reside in the `game` namespace (see `GameState.h`).
- Includes are sorted/regrouped by `clang-format`; keep headers self-contained.

## Testing Guidelines
- Manual playtest is expected: launch the game, confirm movement, HUD updates, pickups, pause, music toggle, and victory/defeat flows.
- When fixing rendering or assets, check that `resources/` is copied to the build output and paths resolve at runtime (`resources/images/...`, `resources/audio/...`, `resources/fonts/...`).

## Commit & Pull Request Guidelines
- Commit messages follow the existing imperative, concise style (e.g., “Refactor game loop into Game class”).
- For PRs, include: a short summary of the change, build/run commands executed, observed results (screenshots/clips for visual tweaks), and linked issues/tasks.
- Call out asset changes explicitly and ensure binary files remain in the asset folders, not the source tree.
