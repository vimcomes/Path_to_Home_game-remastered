# Path_to_Home_game (remastered)

Cross-platform CMake/SFML (C++17) version of “Path to Home”, adapted from the original Windows/Visual Studio project: https://github.com/vimcomes/Path_to_Home_game

## Features
- Smooth delta-time controls; HUD with progress/fuel bars and life icons.
- Fuel pickups with visible amount and shield bonus (≈35% chance); shield blocks damage and pauses fuel drain.
- Rotating asteroids/bonuses, smoke/flashes, parallax background.
- Cleaned resources/constants (`Resources`, `Config`, `GameState`) for easy tuning.

## Build
```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/bin/path_to_home
```

### Dependencies
- CMake ≥ 3.20
- C++17 compiler
- SFML 2.6.x (via find_package or FetchContent fallback)

### License
Free to use for non-commercial purposes with attribution. Original project: https://github.com/vimcomes/Path_to_Home_game

---

## Path_to_Home_game (ремастеред)

Кроссплатформенная версия игры «Путь домой» на CMake/SFML (C++17). Исходник адаптирован из старого Windows/Visual Studio проекта: https://github.com/vimcomes/Path_to_Home_game

## Особенности
- Плавное управление с delta time, HUD с полосами прогресса/топлива и индикатором жизней.
- Бонусы топлива с видимым объёмом и щит (≈35% шанс); щит блокирует урон и отключает расход топлива.
- Вращающиеся астероиды/бонусы, дым/вспышки, параллакс фон.
- Ресурсы/константы вынесены в код (`Resources`, `Config`, `GameState`) для удобной настройки.

## Сборка
```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/bin/path_to_home
```

### Зависимости
- CMake ≥ 3.20
- Компилятор C++17
- SFML 2.6.x (через find_package или FetchContent)

### Лицензия
Код и ассеты доступны для некоммерческого использования (free to use) при сохранении ссылок на авторов. Старый проект: https://github.com/vimcomes/Path_to_Home_game

## Управление
- WASD/стрелки (Numpad) — полёт
- M — вкл/выкл музыку
- P — пауза
- F1 — скип к финишу (debug)
- Щит/топливо подбираются при столкновении с бонусом

## Авторство и история
- Оригинальный проект (Windows/VS): https://github.com/vimcomes/Path_to_Home_game
- Эта версия — переработка под CMake/SFML, с улучшенным геймплеем и эффектами.
