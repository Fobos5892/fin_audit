#include "SettingsViewModel.h"

#include "core/SettingsStore.h"
#include "core/ThemeManager.h"
#include "database/DatabaseManager.h"

#include <QFutureWatcher>

SettingsViewModel::SettingsViewModel(QObject *parent)
    : QObject(parent)
    , m_categories(new CategoryListModel(this))
    , m_incomeCategories(new CategoryListModel(this))
    , m_expenseCategories(new CategoryListModel(this))
{
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &SettingsViewModel::themeChanged);
    connect(&ThemeManager::instance(), &ThemeManager::privacyModeChanged, this, &SettingsViewModel::privacyChanged);
    connect(&SettingsStore::instance(), &SettingsStore::hideChromeChanged, this, &SettingsViewModel::chromeChanged);
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &SettingsViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &SettingsViewModel::refresh);
    if (DatabaseManager::instance().isReady())
        refresh();
}

bool SettingsViewModel::isDarkMode() const
{
    return ThemeManager::instance().isDarkMode();
}

void SettingsViewModel::setDarkMode(bool v)
{
    ThemeManager::instance().setDarkMode(v);
    DatabaseManager::instance().setSetting(QStringLiteral("dark_mode"), v ? QStringLiteral("1") : QStringLiteral("0"));
}

bool SettingsViewModel::isPrivacyMode() const
{
    return ThemeManager::instance().isPrivacyMode();
}

void SettingsViewModel::setPrivacyMode(bool v)
{
    ThemeManager::instance().setPrivacyMode(v);
}

bool SettingsViewModel::hideChromeForRecording() const
{
    return SettingsStore::instance().hideChromeForRecording();
}

void SettingsViewModel::setHideChromeForRecording(bool v)
{
    SettingsStore::instance().setHideChromeForRecording(v);
}

void SettingsViewModel::splitCategories(const QVariantList &items)
{
    QVariantList income;
    QVariantList expense;
    for (const auto &v : items) {
        const auto m = v.toMap();
        ThemeManager::instance().setCategoryColorCache(
            m.value(QStringLiteral("id")).toInt(),
            m.value(QStringLiteral("chartColor")).toString());
        if (m.value(QStringLiteral("type")).toString() == QLatin1String("income"))
            income.append(v);
        else
            expense.append(v);
    }
    m_categories->setItems(items);
    m_incomeCategories->setItems(income);
    m_expenseCategories->setItems(expense);
}

void SettingsViewModel::refresh()
{
    auto *watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        splitCategories(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().fetchCategories());
}

void SettingsViewModel::updateCategoryColor(int categoryId, const QString &hexColor)
{
    ThemeManager::instance().setCategoryColorCache(categoryId, hexColor);
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result(), QStringLiteral("Цвет категории обновлён"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().updateCategoryColor(categoryId, hexColor));
}

void SettingsViewModel::addCategory(const QString &name, bool isIncome, const QString &hexColor)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        emit operationFinished(false, QStringLiteral("Введите название категории"));
        return;
    }
    const QString type = isIncome ? QStringLiteral("income") : QStringLiteral("expense");
    const QString color = hexColor.isEmpty()
                              ? (isIncome ? QStringLiteral("#7C6BC4") : QStringLiteral("#D06B6B"))
                              : hexColor;

    auto *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, isIncome]() {
        const int id = watcher->result();
        emit operationFinished(id > 0,
                               id > 0 ? (isIncome ? QStringLiteral("Категория дохода добавлена")
                                                  : QStringLiteral("Категория расхода добавлена"))
                                      : QStringLiteral("Не удалось добавить категорию"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().addCategory(
        trimmed, type, QStringLiteral("default"), color, -1));
}

void SettingsViewModel::removeCategory(int categoryId)
{
    if (categoryId <= 0) {
        emit operationFinished(false, QStringLiteral("Категория не выбрана"));
        return;
    }
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        const bool ok = watcher->result();
        emit operationFinished(ok, ok ? QStringLiteral("Категория удалена")
                                      : QStringLiteral("Не удалось удалить категорию"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().deleteCategory(categoryId));
}
