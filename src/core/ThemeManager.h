#pragma once

#include <QColor>
#include <QHash>
#include <QObject>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isDarkMode READ isDarkMode WRITE setDarkMode NOTIFY themeChanged)
    Q_PROPERTY(bool isPrivacyMode READ isPrivacyMode WRITE setPrivacyMode NOTIFY privacyModeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor surfaceColor READ surfaceColor NOTIFY themeChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY themeChanged)
    Q_PROPERTY(QColor mutedTextColor READ mutedTextColor NOTIFY themeChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY themeChanged)
    Q_PROPERTY(QColor selectionBgColor READ selectionBgColor NOTIFY themeChanged)
    Q_PROPERTY(QColor selectionBorderColor READ selectionBorderColor NOTIFY themeChanged)
    Q_PROPERTY(QColor selectionTextColor READ selectionTextColor NOTIFY themeChanged)
    Q_PROPERTY(QColor successColor READ successColor NOTIFY themeChanged)
    Q_PROPERTY(QColor dangerColor READ dangerColor NOTIFY themeChanged)
    Q_PROPERTY(QColor borderColor READ borderColor NOTIFY themeChanged)

public:
    static ThemeManager &instance();

    bool isDarkMode() const { return m_isDarkMode; }
    void setDarkMode(bool dark);

    bool isPrivacyMode() const { return m_isPrivacyMode; }
    void setPrivacyMode(bool privacy);

    QColor backgroundColor() const;
    QColor surfaceColor() const;
    QColor textColor() const;
    QColor mutedTextColor() const;
    QColor accentColor() const;
    QColor selectionBgColor() const;
    QColor selectionBorderColor() const;
    QColor selectionTextColor() const;
    QColor successColor() const;
    QColor dangerColor() const;
    QColor borderColor() const;

    Q_INVOKABLE QString getCategoryColor(int categoryId, const QString &defaultHex = QStringLiteral("#6B90B5")) const;
    Q_INVOKABLE void setCategoryColorCache(int categoryId, const QString &hex);
    Q_INVOKABLE void clearCategoryColorCache();

signals:
    void themeChanged();
    void privacyModeChanged();
    void categoryColorChanged(int categoryId, const QString &newColor);

private:
    explicit ThemeManager(QObject *parent = nullptr);

    bool m_isDarkMode = true;
    bool m_isPrivacyMode = false;
    QHash<int, QString> m_categoryColors;
};
