#include "SettingsStore.h"

#include "database/DatabaseManager.h"

#include "core/ThemeManager.h"

SettingsStore &SettingsStore::instance()
{
    static SettingsStore inst;
    return inst;
}

SettingsStore::SettingsStore(QObject *parent)
    : QObject(parent)
{
}

void SettingsStore::setHideChromeForRecording(bool hide)
{
    if (m_hideChrome == hide)
        return;
    m_hideChrome = hide;
    persistKey(QStringLiteral("hide_chrome"), hide ? QStringLiteral("1") : QStringLiteral("0"));
    emit hideChromeChanged();
}

void SettingsStore::loadFromDatabase()
{
    DatabaseManager::instance().runOnReady([this]() {
        const auto map = DatabaseManager::instance().loadSettingsSync();
        const bool hide = map.value(QStringLiteral("hide_chrome")) == QLatin1String("1");
        if (m_hideChrome != hide) {
            m_hideChrome = hide;
            emit hideChromeChanged();
        }
        const bool dark = map.value(QStringLiteral("dark_mode"), QStringLiteral("1")) != QLatin1String("0");
        ThemeManager::instance().setDarkMode(dark);
    });
}

void SettingsStore::persistKey(const QString &key, const QString &value)
{
    DatabaseManager::instance().setSetting(key, value);
}
