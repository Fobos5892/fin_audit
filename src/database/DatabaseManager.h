#pragma once

#include <QFuture>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <functional>

class QSqlDatabase;

class DatabaseManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ isReady NOTIFY databaseReady)

public:
    static DatabaseManager &instance();

    void initialize(const QString &dbPath);
    bool isReady() const { return m_ready; }
    QString dbPath() const { return m_dbPath; }

    void runOnReady(const std::function<void()> &fn);

    QHash<QString, QString> loadSettingsSync();
    void setSetting(const QString &key, const QString &value);

    QFuture<QVariantList> fetchAccounts();
    QFuture<int> addAccount(const QString &name, const QString &type, qint64 balanceCents,
                            const QString &currency = QStringLiteral("RUB"));

    QFuture<QVariantList> fetchCategories();
    QFuture<int> addCategory(const QString &name, const QString &type, const QString &icon,
                             const QString &color, int parentId = -1);
    QFuture<bool> updateCategoryColor(int id, const QString &hex);
    QFuture<bool> deleteCategory(int id);

    QFuture<QVariantList> fetchTransactions(int limit = 200);
    QFuture<int> addTransaction(int accountId, int categoryId, qint64 amountCents,
                                const QString &dateIso, const QString &description,
                                const QString &ownership, bool isInvestment);
    QFuture<bool> deleteTransaction(int id);

    QFuture<QVariantList> fetchGoals();
    QFuture<int> addGoal(const QString &name, qint64 targetCents, qint64 currentCents,
                         const QString &deadline, bool autoAllocate);
    QFuture<bool> updateGoalProgress(int id, qint64 currentCents);
    QFuture<bool> contributeToGoal(int id, qint64 addCents);
    QFuture<bool> allocateToGoals(qint64 amountCents);

    QFuture<QVariantList> fetchInvestments();
    QFuture<int> upsertInvestmentTrade(const QString &ticker, const QString &name, const QString &type,
                                       const QString &side, double quantity, qint64 priceCents,
                                       const QString &dateIso);
    QFuture<bool> updateInvestmentPrice(int id, qint64 currentPriceCents);

    QFuture<QVariantList> fetchMortgages();
    QFuture<int> saveMortgage(const QVariantMap &data);
    QFuture<bool> replaceMortgageSchedule(int mortgageId, const QVariantList &rows, bool withExtra);

    QFuture<QVariantList> expenseSlicesForMonth(int year, int month);
    QFuture<QVariantList> incomeSlicesForMonth(int year, int month);
    QFuture<QVariantMap> monthTotals(int year, int month);
    QFuture<qint64> totalAccountsBalance();
    QFuture<qint64> totalGoalsCurrent();
    QFuture<qint64> totalPortfolioValue();
    QFuture<qint64> totalMortgageRemaining();

signals:
    void databaseReady();
    void dataChanged();
    void errorOccurred(const QString &message);

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    bool executeSchema(QSqlDatabase &db);
    bool seedIfEmpty(QSqlDatabase &db);
    bool withDb(const std::function<bool(QSqlDatabase &)> &op) const;

    template <typename T>
    QFuture<T> runAsync(const std::function<T()> &fn);

    QString m_dbPath;
    bool m_ready = false;
    QList<std::function<void()>> m_pendingReady;
};
