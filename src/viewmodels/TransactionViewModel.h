#pragma once

#include <QObject>
#include <atomic>

#include "models/AccountListModel.h"
#include "models/CategoryListModel.h"
#include "models/TransactionListModel.h"

class TransactionViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TransactionListModel *transactions READ transactions CONSTANT)
    Q_PROPERTY(TransactionListModel *incomeTransactions READ incomeTransactions CONSTANT)
    Q_PROPERTY(TransactionListModel *expenseTransactions READ expenseTransactions CONSTANT)
    Q_PROPERTY(AccountListModel *accounts READ accounts CONSTANT)
    Q_PROPERTY(CategoryListModel *categories READ categories CONSTANT)
    Q_PROPERTY(CategoryListModel *incomeCategories READ incomeCategories CONSTANT)
    Q_PROPERTY(CategoryListModel *expenseCategories READ expenseCategories CONSTANT)
    Q_PROPERTY(QString filterOwnership READ filterOwnership WRITE setFilterOwnership NOTIFY filterChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY operationFinished)

public:
    explicit TransactionViewModel(QObject *parent = nullptr);

    TransactionListModel *transactions() const { return m_transactions; }
    TransactionListModel *incomeTransactions() const { return m_incomeTransactions; }
    TransactionListModel *expenseTransactions() const { return m_expenseTransactions; }
    AccountListModel *accounts() const { return m_accounts; }
    CategoryListModel *categories() const { return m_categories; }
    CategoryListModel *incomeCategories() const { return m_incomeCategories; }
    CategoryListModel *expenseCategories() const { return m_expenseCategories; }
    QString filterOwnership() const { return m_filterOwnership; }
    void setFilterOwnership(const QString &v);
    QString lastError() const { return m_lastError; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void submitTransaction(int accountIndex, int categoryIndex, const QString &amountText,
                                       const QString &description, const QString &ownership, bool isIncome);
    Q_INVOKABLE void removeTransaction(int id);
    Q_INVOKABLE int accountIdAt(int row) const;
    Q_INVOKABLE int categoryIdAt(int row) const;
    Q_INVOKABLE int incomeCategoryIdAt(int row) const;
    Q_INVOKABLE int expenseCategoryIdAt(int row) const;

signals:
    void filterChanged();
    void operationFinished(bool ok, const QString &message);

private:
    void applyFilter(const QVariantList &all);
    void splitCategories(const QVariantList &all);
    static int idAt(CategoryListModel *model, int row);

    TransactionListModel *m_transactions = nullptr;
    TransactionListModel *m_incomeTransactions = nullptr;
    TransactionListModel *m_expenseTransactions = nullptr;
    AccountListModel *m_accounts = nullptr;
    CategoryListModel *m_categories = nullptr;
    CategoryListModel *m_incomeCategories = nullptr;
    CategoryListModel *m_expenseCategories = nullptr;
    QString m_filterOwnership;
    QString m_lastError;
    QVariantList m_allTransactions;
    std::atomic<quint64> m_refreshGen{0};
};
