#include "ThemeManager.h"

ThemeManager &ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
}

void ThemeManager::setDarkMode(bool dark)
{
    if (m_isDarkMode == dark)
        return;
    m_isDarkMode = dark;
    emit themeChanged();
}

void ThemeManager::setPrivacyMode(bool privacy)
{
    if (m_isPrivacyMode == privacy)
        return;
    m_isPrivacyMode = privacy;
    emit privacyModeChanged();
}

QColor ThemeManager::backgroundColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#0F172A")) : QColor(QStringLiteral("#F8FAFC"));
}

QColor ThemeManager::surfaceColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#1E293B")) : QColor(QStringLiteral("#FFFFFF"));
}

QColor ThemeManager::textColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#F1F5F9")) : QColor(QStringLiteral("#0F172A"));
}

QColor ThemeManager::mutedTextColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#94A3B8")) : QColor(QStringLiteral("#64748B"));
}

QColor ThemeManager::accentColor() const
{
    // Muted steel-blue (finance / SaaS sidebars), not neon #3B82F6
    return m_isDarkMode ? QColor(QStringLiteral("#6B90B5"))
                        : QColor(QStringLiteral("#3E6A8F"));
}

QColor ThemeManager::selectionBgColor() const
{
    // Soft lift over surface — ~10–15% white tint pattern
    return m_isDarkMode ? QColor(QStringLiteral("#2A3648"))
                        : QColor(QStringLiteral("#E8EEF4"));
}

QColor ThemeManager::selectionBorderColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#3F5166"))
                        : QColor(QStringLiteral("#C5D0DC"));
}

QColor ThemeManager::selectionTextColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#D7E3EE"))
                        : QColor(QStringLiteral("#1E3A5F"));
}

QColor ThemeManager::successColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#3FAE8A"))
                        : QColor(QStringLiteral("#0F8A66"));
}

QColor ThemeManager::dangerColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#D06B6B"))
                        : QColor(QStringLiteral("#C0392B"));
}

QColor ThemeManager::borderColor() const
{
    return m_isDarkMode ? QColor(QStringLiteral("#334155")) : QColor(QStringLiteral("#E2E8F0"));
}

QString ThemeManager::getCategoryColor(int categoryId, const QString &defaultHex) const
{
    return m_categoryColors.value(categoryId, defaultHex);
}

void ThemeManager::setCategoryColorCache(int categoryId, const QString &hex)
{
    m_categoryColors.insert(categoryId, hex);
    emit categoryColorChanged(categoryId, hex);
}

void ThemeManager::clearCategoryColorCache()
{
    m_categoryColors.clear();
}
