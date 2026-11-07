#include "AnalyticsService.h"

#include "database/DatabaseManager.h"

#include <QDate>
#include <QFutureWatcher>

AnalyticsService::AnalyticsService(QObject *parent)
    : QObject(parent)
{
}

void AnalyticsService::requestMonthAnalytics(int year, int month)
{
    auto *totalsWatcher = new QFutureWatcher<QVariantMap>(this);
    connect(totalsWatcher, &QFutureWatcherBase::finished, this, [this, totalsWatcher, year, month]() {
        const QVariantMap totals = totalsWatcher->result();
        totalsWatcher->deleteLater();

        auto *expenseWatcher = new QFutureWatcher<QVariantList>(this);
        connect(expenseWatcher, &QFutureWatcherBase::finished, this,
                [this, expenseWatcher, totals, year, month]() {
                    const QVariantList expenseSlices = expenseWatcher->result();
                    expenseWatcher->deleteLater();

                    auto *incomeWatcher = new QFutureWatcher<QVariantList>(this);
                    connect(incomeWatcher, &QFutureWatcherBase::finished, this,
                            [this, incomeWatcher, totals, expenseSlices]() {
                                emit monthAnalyticsReady(totals, expenseSlices, incomeWatcher->result());
                                incomeWatcher->deleteLater();
                            });
                    incomeWatcher->setFuture(DatabaseManager::instance().incomeSlicesForMonth(year, month));
                });
        expenseWatcher->setFuture(DatabaseManager::instance().expenseSlicesForMonth(year, month));
    });
    totalsWatcher->setFuture(DatabaseManager::instance().monthTotals(year, month));
}

void AnalyticsService::requestNetWorth()
{
    auto *accWatcher = new QFutureWatcher<qint64>(this);
    connect(accWatcher, &QFutureWatcherBase::finished, this, [this, accWatcher]() {
        const qint64 accounts = accWatcher->result();
        accWatcher->deleteLater();

        auto *portWatcher = new QFutureWatcher<qint64>(this);
        connect(portWatcher, &QFutureWatcherBase::finished, this, [this, accounts, portWatcher]() {
            const qint64 portfolio = portWatcher->result();
            portWatcher->deleteLater();

            auto *mortWatcher = new QFutureWatcher<qint64>(this);
            connect(mortWatcher, &QFutureWatcherBase::finished, this, [this, accounts, portfolio, mortWatcher]() {
                const qint64 mortgage = mortWatcher->result();
                mortWatcher->deleteLater();

                auto *goalWatcher = new QFutureWatcher<qint64>(this);
                connect(goalWatcher, &QFutureWatcherBase::finished, this,
                        [this, accounts, portfolio, mortgage, goalWatcher]() {
                            const qint64 goals = goalWatcher->result();
                            QVariantMap parts;
                            parts.insert(QStringLiteral("accountsCents"), accounts);
                            parts.insert(QStringLiteral("portfolioCents"), portfolio);
                            parts.insert(QStringLiteral("mortgageCents"), mortgage);
                            parts.insert(QStringLiteral("goalsCents"), goals);
                            parts.insert(QStringLiteral("netWorthCents"), accounts + portfolio - mortgage);
                            emit netWorthReady(accounts + portfolio - mortgage, parts);
                            goalWatcher->deleteLater();
                        });
                goalWatcher->setFuture(DatabaseManager::instance().totalGoalsCurrent());
            });
            mortWatcher->setFuture(DatabaseManager::instance().totalMortgageRemaining());
        });
        portWatcher->setFuture(DatabaseManager::instance().totalPortfolioValue());
    });
    accWatcher->setFuture(DatabaseManager::instance().totalAccountsBalance());
}
