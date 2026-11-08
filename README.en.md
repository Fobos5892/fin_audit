# Fin Audit / Family Budget

Desktop family budget app built with **Qt 6** (QML UI + C++20 core) and **SQLite**.

[← Menu](README.md) · [Русская версия](README.ru.md)

## Features

- **Dashboard** — monthly summary, recent income/expense lists, category structure charts, budget ring
- **Operations** — separate income and expense forms, category filters, family/personal ownership
- **Goals** — savings targets with progress and optional auto-allocation
- **Investments** — portfolio positions (stocks, bonds, crypto, fiat, metals) and P&L
- **Mortgage** — schedule, overpayment scenarios, remaining principal and interest charts
- **Capital** — consolidated view and growth chart
- **Settings** — themes (light/dark), language (RU/EN), privacy mode (hide amounts), category management
- **MVVM architecture** — ViewModels + SQLite DAOs, charts rendered in QML

## Requirements

- Qt **6.5+** (tested with 6.11.1): Core, Gui, Qml, Quick, QuickControls2, Sql, Concurrent, Test
- CMake **≥ 3.21**
- C++20 compiler (MinGW 13 / MSVC 2022 / GCC / Clang)
- Ninja (recommended)

SQLite is embedded via Qt Sql — no external database server.

### Windows (example paths)

Install [Qt Online Installer](https://www.qt.io/download) with MinGW or MSVC kit, then adjust paths in `CMakePresets.json` if needed.

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake ninja-build \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libqt6sql6-sqlite
```

## Build

### Presets (Windows / Qt Creator)

```bash
cmake --preset default
cmake --build --preset default
./build/FamilyBudget   # or build\FamilyBudget.exe on Windows
```

Other presets: `windows-mingw-release`, `windows-mingw-debug`, `windows-msvc-release`.

### Manual

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build -j
./build/FamilyBudget
```

## Tests

```bash
cmake --build build --target all
ctest --test-dir build --output-on-failure
# or: cmake --build --preset default && ctest --preset default
```

## Installer (Windows)

Requires [Inno Setup 6](https://jrsoftware.org/isinfo.php): `winget install JRSoftware.InnoSetup`.

```powershell
powershell -ExecutionPolicy Bypass -File scripts\package-windows.ps1
```

The script builds Release, collects the exe with Qt dependencies via `windeployqt` into `build\dist\FinAudit`, and compiles `installer\fin_audit.iss` into `build\installer\FinAudit-Setup-<version>.exe`.

- `-SkipBuild` — reuse the existing exe
- `-SkipInstaller` — portable folder only
- `-QtDir`, `-MinGWDir`, `-Preset` — custom Qt/compiler paths

User data (`family_budget.db`) lives in `%APPDATA%` and is kept on uninstall.

## First run

1. Launch `FamilyBudget`.
2. SQLite DB is created automatically (local app data).
3. Add accounts/categories via **Settings**, then record operations.
4. Toggle **privacy mode** and **theme** from the nav rail; language in **Settings**.

## Project layout

```
src/core/          settings, theme, splash, money formatting
src/database/      SQLite manager and DAO helpers
src/models/        QAbstractListModel wrappers
src/services/      analytics, mortgage, portfolio, goals
src/viewmodels/    MVVM layer exposed to QML
qml/components/    shared UI (theme, charts, controls)
qml/views/         main screens
sql/migrations/    SQLite schema
resources/         icons, translations (.ts)
tests/             Qt Test unit tests
```

## License

See repository for license terms. Project: [Fobos5892/fin_audit](https://github.com/Fobos5892/fin_audit).
