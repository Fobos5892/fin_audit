#!/usr/bin/env bash
# Download Qt QML/PostgreSQL packages into .deps (no sudo).
# No Widgets / Charts — app is QGuiApplication + Quick Controls 2 only.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEPS="$ROOT/.deps"
TMP="${TMPDIR:-/tmp}/fin_audit_qt_deps_$$"
mkdir -p "$TMP" "$DEPS"

PACKAGES=(
  qtdeclarative5-dev
  qtquickcontrols2-5-dev
  qttools5-dev
  libqt5sql5-psql
  libqt5quickcontrols2-5
  libqt5quicktemplates2-5
  libqt5qmlmodels5
  libqt5qmlworkerscript5
  libqt5qml5
  libqt5quick5
  qt5-qmltooling-plugins
  qtdeclarative5-dev-tools
  qml-module-qtquick2
  qml-module-qtquick-controls2
  qml-module-qtquick-layouts
  qml-module-qtquick-templates2
  qml-module-qtquick-window2
  qml-module-qtqml
  qml-module-qtqml-models2
)

echo "Downloading packages into $TMP ..."
cd "$TMP"
apt-get download "${PACKAGES[@]}"

echo "Extracting into $DEPS ..."
for deb in *.deb; do
  dpkg-deb -x "$deb" "$DEPS"
done

# Symlink system lrelease if LinguistTools config expects it under .deps
mkdir -p "$DEPS/usr/lib/qt5/bin"
if [[ -x /usr/bin/lrelease ]]; then
  ln -sfn /usr/bin/lrelease "$DEPS/usr/lib/qt5/bin/lrelease"
fi
if [[ -x /usr/bin/lupdate ]]; then
  ln -sfn /usr/bin/lupdate "$DEPS/usr/lib/qt5/bin/lupdate"
fi

# Stub any missing qmltooling plugins referenced by CMake configs
python3 - <<PY
import os, re
deps = "$DEPS"
base = os.path.join(deps, "usr/lib/x86_64-linux-gnu")
cfg = os.path.join(base, "cmake", "Qt5Qml")
if os.path.isdir(cfg):
    for root, _, files in os.walk(cfg):
        for f in files:
            if not f.endswith(".cmake"):
                continue
            text = open(os.path.join(root, f), encoding="utf-8", errors="ignore").read()
            for m in re.findall(r'"([^"]+\.so)"', text):
                p = m
                if not p.startswith("/"):
                    p = os.path.join(base, "qt5", "plugins", m)
                if not os.path.exists(p) and "qmltooling" in p:
                    os.makedirs(os.path.dirname(p), exist_ok=True)
                    open(p, "ab").close()
PY

rm -rf "$TMP"
echo "Done. Re-run CMake configure in Qt Creator (or: cmake -B build)."
echo "Preferred (system-wide): sudo apt install qtdeclarative5-dev qtquickcontrols2-5-dev libqt5charts5-dev libqt5sql5-psql"
