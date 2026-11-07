#pragma once

#include <QObject>
#include <QVariantList>
#include <atomic>

#include "models/ChartSliceModel.h"
#include "models/GoalListModel.h"
#include "models/TransactionListModel.h"
#include "services/AnalyticsService.h"

class DashboardViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 balanceCents READ balanceCents NOTIFY totalsChanged)
    Q_PROPERTY(qint64 incomeCents READ incomeCents NOTIFY totalsChanged)
    Q_PROPERTY(qint64 expenseCents READ expenseCents NOTIFY totalsChanged)
    Q_PROPERTY(qint64 portfolioCents READ portfolioCents NOTIFY totalsChanged)
    Q_PROPERTY(qint64 workIncomeCents READ workIncomeCents NOTIFY totalsChanged)
    Q_PROPERTY(qint64 passiveIncomeCents READ passiveIncomeCents NOTIFY totalsChanged)
    Q_PROPERTY(double expenseToIncomePct READ expenseToIncomePct NOTIFY totalsChanged)
    Q_PROPERTY(QVariantList expenseDonutSlices READ expenseDonutSlices NOTIFY totalsChanged)
    Q_PROPERTY(QVariantList incomeDonutSlices READ incomeDonutSlices NOTIFY totalsChanged)
    Q_PROPERTY(QVariantList donutSlices READ expenseDonutSlices NOTIFY totalsChanged) // compat
    Q_PROPERTY(TransactionListModel *transactions READ transactions CONSTANT)
    Q_PROPERTY(TransactionListModel *incomeTransactions READ incomeTransactions CONSTANT)
    Q_PROPERTY(TransactionListModel *expenseTransactions READ expenseTransactions CONSTANT)
    Q_PROPERTY(ChartSliceModel *expenseSlices READ expenseSlices CONSTANT)
    Q_PROPERTY(GoalListModel *goals READ goals CONSTANT)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit DashboardViewModel(QObject *parent = nullptr);

    qint64 balanceCents() const { return m_balance; }
    qint64 incomeCents() const { return m_income; }
    qint64 expenseCents() const { return m_expense; }
    qint64 portfolioCents() const { return m_portfolio; }
    qint64 workIncomeCents() const { return m_workIncome; }
    qint64 passiveIncomeCents() const { return m_passiveIncome; }
    double expenseToIncomePct() const;
    QVariantList expenseDonutSlices() const { return m_expenseDonut; }
    QVariantList incomeDonutSlices() const { return m_incomeDonut; }
    TransactionListModel *transactions() const { return m_transactions; }
    TransactionListModel *incomeTransactions() const { return m_incomeTransactions; }
    TransactionListModel *expenseTransactions() const { return m_expenseTransactions; }
    ChartSliceModel *expenseSlices() const { return m_slices; }
    GoalListModel *goals() const { return m_goals; }
    bool loading() const { return m_loading; }

    Q_INVOKABLE void refresh();

signals:
    void totalsChanged();
    void loadingChanged();

private:
    void setLoading(bool v);
    static QVariantList toDonutList(const QVariantList &raw);

    TransactionListModel *m_transactions = nullptr;
    TransactionListModel *m_incomeTransactions = nullptr;
    TransactionListModel *m_expenseTransactions = nullptr;
    ChartSliceModel *m_slices = nullptr;
    GoalListModel *m_goals = nullptr;
    AnalyticsService *m_analytics = nullptr;
    QVariantList m_expenseDonut;
    QVariantList m_incomeDonut;
    qint64 m_balance = 0;
    qint64 m_income = 0;
    qint64 m_expense = 0;
    qint64 m_portfolio = 0;
    qint64 m_workIncome = 0;
    qint64 m_passiveIncome = 0;
    bool m_loading = false;
    std::atomic<quint64> m_refreshGen{0};
};
