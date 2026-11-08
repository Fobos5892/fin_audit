# Fin Audit / Семейный бюджет

Десктопное приложение семейного бюджета на **Qt 6** (QML UI + ядро C++20) и **SQLite**.

[← Меню](README.md) · [English version](README.en.md)

## Возможности

- **Дашборд** — итоги месяца, списки доходов/расходов, структура по категориям, кольцо бюджета
- **Операции** — отдельные формы доходов и расходов, фильтры по категориям, владение семья/личное
- **Цели** — накопления с прогрессом и опциональным автораспределением
- **Инвестиции** — портфель (акции, облигации, крипто, фиат, металлы) и P&L
- **Ипотека** — график платежей, досрочные погашения, графики остатка и процентов
- **Капитал** — сводный обзор и график роста
- **Настройки** — темы (светлая/тёмная), язык (RU/EN), режим приватности (скрытие сумм), управление категориями
- **Архитектура MVVM** — ViewModels + DAO для SQLite, графики на QML

## Зависимости

- Qt **6.5+** (проверено на 6.11.1): Core, Gui, Qml, Quick, QuickControls2, Sql, Concurrent, Test
- CMake **≥ 3.21**
- Компилятор C++20 (MinGW 13 / MSVC 2022 / GCC / Clang)
- Ninja (рекомендуется)

SQLite встроен через Qt Sql — отдельный сервер БД не нужен.

### Windows (пример путей)

Установите [Qt Online Installer](https://www.qt.io/download) с комплектом MinGW или MSVC и при необходимости поправьте пути в `CMakePresets.json`.

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake ninja-build \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libqt6sql6-sqlite
```

## Сборка

### Пресеты (Windows / Qt Creator)

```bash
cmake --preset default
cmake --build --preset default
./build/FamilyBudget   # или build\FamilyBudget.exe в Windows
```

Другие пресеты: `windows-mingw-release`, `windows-mingw-debug`, `windows-msvc-release`.

### Вручную

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build -j
./build/FamilyBudget
```

## Тесты

```bash
cmake --build build --target all
ctest --test-dir build --output-on-failure
# или: cmake --build --preset default && ctest --preset default
```

## Первый запуск

1. Запустите `FamilyBudget`.
2. База SQLite создаётся автоматически в локальных данных приложения.
3. Добавьте счета/категории в **Настройках**, затем заносите операции.
4. Режим **приватности** и **тему** переключайте на панели навигации; язык — в **Настройках**.

## Структура проекта

```
src/core/          настройки, тема, splash, форматирование денег
src/database/      менеджер SQLite и DAO
src/models/        обёртки QAbstractListModel
src/services/      аналитика, ипотека, портфель, цели
src/viewmodels/    слой MVVM для QML
qml/components/    общие UI-компоненты (тема, графики, контролы)
qml/views/         основные экраны
sql/migrations/    схема SQLite
resources/         иконки, переводы (.ts)
tests/             модульные тесты Qt Test
```

## Лицензия

См. условия в репозитории. Проект: [Fobos5892/fin_audit](https://github.com/Fobos5892/fin_audit).
