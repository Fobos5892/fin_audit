#pragma once

#include <QObject>

#include "models/CategoryListModel.h"

class SettingsViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CategoryListModel *categories READ categories CONSTANT)
    Q_PROPERTY(CategoryListModel *incomeCategories READ incomeCategories CONSTANT)
    Q_PROPERTY(CategoryListModel *expenseCategories READ expenseCategories CONSTANT)
    Q_PROPERTY(bool isDarkMode READ isDarkMode WRITE setDarkMode NOTIFY themeChanged)
    Q_PROPERTY(bool isPrivacyMode READ isPrivacyMode WRITE setPrivacyMode NOTIFY privacyChanged)
    Q_PROPERTY(bool hideChromeForRecording READ hideChromeForRecording WRITE setHideChromeForRecording NOTIFY chromeChanged)

public:
    explicit SettingsViewModel(QObject *parent = nullptr);

    CategoryListModel *categories() const { return m_categories; }
    CategoryListModel *incomeCategories() const { return m_incomeCategories; }
    CategoryListModel *expenseCategories() const { return m_expenseCategories; }

    bool isDarkMode() const;
    void setDarkMode(bool v);
    bool isPrivacyMode() const;
    void setPrivacyMode(bool v);
    bool hideChromeForRecording() const;
    void setHideChromeForRecording(bool v);

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void updateCategoryColor(int categoryId, const QString &hexColor);
    Q_INVOKABLE void addCategory(const QString &name, bool isIncome, const QString &hexColor = QString());
    Q_INVOKABLE void removeCategory(int categoryId);

signals:
    void themeChanged();
    void privacyChanged();
    void chromeChanged();
    void operationFinished(bool ok, const QString &message);

private:
    void splitCategories(const QVariantList &items);

    CategoryListModel *m_categories = nullptr;
    CategoryListModel *m_incomeCategories = nullptr;
    CategoryListModel *m_expenseCategories = nullptr;
};
