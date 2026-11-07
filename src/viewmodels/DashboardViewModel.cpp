#include "DashboardViewModel.h"

#include "core/ThemeManager.h"
#include "database/DatabaseManager.h"

#include <QDate>
#include <QFutureWatcher>
#include <QtMath>

DashboardViewModel::DashboardViewModel(QObject *parent)
    : QObject(parent)
    , m_transactions(new TransactionListModel(this))
    , m_incomeTransactions(new TransactionListModel(this))
    , m_expenseTransactions(new TransactionListModel(this))
    , m_slices(new ChartSliceModel(this))
    , m_goals(new GoalListModel(this))
    , m_analytics(new AnalyticsService(this))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &DashboardViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &DashboardViewModel::refresh);
    connect(m_analytics, &AnalyticsService::monthAnalyticsReady, this,
            [this](const QVariantMap &totals, const QVariantList &expenseSlices,
                   const QVariantList &incomeSlices) {
                m_income = totals.value(QStringLiteral("incomeCents")).toLongLong();
                m_expense = totals.value(QStringLiteral("expenseCents")).toLongLong();
                m_workIncome = totals.value(QStringLiteral("workIncomeCents")).toLongLong();
                m_passiveIncome = totals.value(QStringLiteral("passiveIncomeCents")).toLongLong();
                m_slices->setItems(expenseSlices);
                m_expenseDonut = toDonutList(expenseSlices);
                m_incomeDonut = toDonutList(incomeSlices);
                for (const auto &v : expenseSlices) {
                    const auto m = v.toMap();
                    ThemeManager::instance().setCategoryColorCache(
                        m.value(QStringLiteral("categoryId")).toInt(),
                        m.value(QStringLiteral("color")).toString());
                }
                emit totalsChanged();
                setLoading(false);
            });
    connect(m_analytics, &AnalyticsService::netWorthReady, this,
            [this](qint64, const QVariantMap &parts) {
                m_balance = parts.value(QStringLiteral("accountsCents")).toLongLong();
                m_portfolio = parts.value(QStringLiteral("portfolioCents")).toLongLong();
                emit totalsChanged();
            });

    if (DatabaseManager::instance().isReady())
        refresh();
}

double DashboardViewModel::expenseToIncomePct() const
{
    const qint64 positivePassive = qMax<qint64>(0, m_passiveIncome);
    const qint64 budget = m_workIncome + positivePassive;
    if (budget <= 0)
        return m_expense > 0 ? 999.0 : 0.0;
    return (static_cast<double>(m_expense) / static_cast<double>(budget)) * 100.0;
}

QVariantList DashboardViewModel::toDonutList(const QVariantList &raw)
{
    QVariantList out;
    for (const auto &v : raw) {
        const auto m = v.toMap();
        const qint64 cents = m.value(QStringLiteral("valueCents")).toLongLong();
        if (cents == 0)
            continue;
        QVariantMap slice;
        slice.insert(QStringLiteral("color"), m.value(QStringLiteral("color")));
        slice.insert(QStringLiteral("value"), qAbs(cents));
        slice.insert(QStringLiteral("label"), m.value(QStringLiteral("label")));
        slice.insert(QStringLiteral("kind"), m.value(QStringLiteral("kind")));
        slice.insert(QStringLiteral("signedCents"), cents);
        out.append(slice);
    }
    return out;
}

void DashboardViewModel::setLoading(bool v)
{
    if (m_loading == v)
        return;
    m_loading = v;
    emit loadingChanged();
}

void DashboardViewModel::refresh()
{
    const quint64 gen = ++m_refreshGen;
    setLoading(true);
    const QDate now = QDate::currentDate();
    m_analytics->requestMonthAnalytics(now.year(), now.month());
    m_analytics->requestNetWorth();

    auto *txWatcher = new QFutureWatcher<QVariantList>(this);
    connect(txWatcher, &QFutureWatcherBase::finished, this, [this, txWatcher, gen]() {
        if (gen == m_refreshGen.load()) {
            const QVariantList all = txWatcher->result();
            QVariantList income;
            QVariantList expense;
            for (const QVariant &v : all) {
                if (v.toMap().value(QStringLiteral("amountCents")).toLongLong() >= 0)
                    income.append(v);
                else
                    expense.append(v);
            }
            m_transactions->setItems(all);
            m_incomeTransactions->setItems(income);
            m_expenseTransactions->setItems(expense);
        }
        txWatcher->deleteLater();
    });
    txWatcher->setFuture(DatabaseManager::instance().fetchTransactions(60));

    auto *goalWatcher = new QFutureWatcher<QVariantList>(this);
    connect(goalWatcher, &QFutureWatcherBase::finished, this, [this, goalWatcher, gen]() {
        if (gen == m_refreshGen.load())
            m_goals->setItems(goalWatcher->result());
        goalWatcher->deleteLater();
    });
    goalWatcher->setFuture(DatabaseManager::instance().fetchGoals());
}
