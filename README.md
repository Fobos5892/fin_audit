# Fin Audit

Десктопное приложение семейного бюджета на **Qt 5.15** (QML UI + C++ ядро) с **PostgreSQL**.

## Возможности

- Члены семьи (люди) и их операции
- Доходы и расходы за месяц
- Финансовые цели и взносы
- Отчёты: месяц по категориям, год по месяцам (графики на Qt Quick Canvas)
- Мультиязычный UI: русский по умолчанию, английский в настройках
  - статические строки — `.ts` / `.qm` в ресурсах
  - названия категорий и UI-ключи — таблицы `*_i18n` в PostgreSQL

## Зависимости

- Qt 5.15+: Core, Gui, Qml, Quick, QuickControls2, Sql (`QGuiApplication`, без Widgets/Charts)
- Драйвер PostgreSQL для Qt: `libqt5sql5-psql` (пакет `QPSQL`)
- PostgreSQL server + client libs (`libpq`)
- CMake ≥ 3.16, компилятор C++17

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake ninja-build \
  qtbase5-dev qtdeclarative5-dev qtquickcontrols2-5-dev \
  qml-module-qtquick-controls2 qml-module-qtquick-layouts \
  qml-module-qtquick2 qttools5-dev qttools5-dev-tools \
  libqt5sql5-psql libpq-dev postgresql
```

Важно: нужен именно **`qml-module-qtquick-controls2`** (Controls 2).  
Пакет `qml-module-qtquick-controls` — это Controls 1, его недостаточно.

Если `sudo` недоступен:

```bash
./scripts/fetch-qt-deps.sh
```

## База данных

```bash
sudo -u postgres createuser -P fin_audit   # или используйте существующего пользователя
sudo -u postgres createdb -O fin_audit fin_audit
```

Параметры подключения задаются в приложении: **Настройки → PostgreSQL**.  
По умолчанию: `127.0.0.1:5432`, БД `fin_audit`, пользователь `postgres`.

При успешном подключении приложение применяет схему (`sql/001_schema.sql`) и seed (`sql/002_seed_ru_en.sql`).

## Сборка

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/fin_audit
```

Если Qt установлен в нестандартный путь:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.x/gcc_64
```

## Первый запуск

1. Запустите PostgreSQL и создайте БД.
2. Откройте приложение → **Настройки**.
3. Укажите host/port/db/user/password → **Подключить и применить схему**.
4. Добавьте людей, операции и цели; смотрите дашборд и отчёты.
5. Язык: **Настройки → Язык интерфейса** (`ru` / `en`).

Локальные настройки подключения и язык хранятся в `QSettings` (`FinAudit` / `fin_audit`).

## Структура

```
src/          C++ ядро (Database, repositories, services, models, i18n)
qml/          QML UI (страницы и компоненты)
sql/          схема и seed PostgreSQL
resources/    qrc и переводы
```
