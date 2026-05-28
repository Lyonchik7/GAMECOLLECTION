# 🎮 Game Collection — Классические игры на C++ / Qt

Небольшая коллекция консольных и настольных игр, реализованных на C++ с использованием фреймворка Qt. Проект создан для демонстрации базовых алгоритмов, работы с отрисовкой и логикой классических аркад.

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![Qt](https://img.shields.io/badge/Qt-6.10-green.svg?style=flat&logo=qt)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

---

## 🕹️ Список игр

| Игра | Описание | Файлы |
|------|----------|-------|
| **2048** | Собирайте плитки с одинаковыми числами, чтобы получить 2048 | `Game2048.cpp/h` |
| **Сапёр (Minesweeper)** | Классический сапёр. Открывайте клетки, избегайте мин | `Minessweeper.cpp/h` |
| **Змейка (Snake)** | Управляйте змейкой, ешьте еду и не врезайтесь в себя | `Snake.cpp/h` |
| **Крестики-нолики (Tic-Tac-Toe)** | Игра для двоих на одном устройстве | `TicTacToe.cpp/h` |
| **Simple Mario** | Аркадный платформер (упрощённая версия) | `SimpleMario.cpp/h` |

---

## 🚀 Быстрый старт

### Требования
- **Qt 6.10** или новее
- **C++17** компилятор (MinGW / MSVC / Clang)
- **CMake** 3.16+

### Сборка и запуск

```bash
# Клонируйте репозиторий
git clone https://github.com/your-username/game-collection.git
cd game-collection

# Создайте папку для сборки
mkdir build && cd build

# Сгенерируйте Makefile (укажите путь к Qt)
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.10/gcc_64

# Соберите проект
make

# Запустите
./GameCollection
