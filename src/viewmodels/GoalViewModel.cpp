#include "GoalViewModel.h"

#include "core/MoneyFormatter.h"
#include "database/DatabaseManager.h"
#include "services/GoalAllocator.h"

#include <QFutureWatcher>

GoalViewModel::GoalViewModel(QObject *parent)
    : QObject(parent)
    , m_goals(new GoalListModel(this))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &GoalViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &GoalViewModel::refresh);
    if (DatabaseManager::instance().isReady())
        refresh();
}

void GoalViewModel::refresh()
{
    const quint64 gen = ++m_refreshGen;

    auto *watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, gen]() {
        if (gen == m_refreshGen.load())
            m_goals->setItems(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().fetchGoals());

    auto *balWatcher = new QFutureWatcher<qint64>(this);
    connect(balWatcher, &QFutureWatcherBase::finished, this, [this, balWatcher, gen]() {
        const qint64 balance = balWatcher->result();
        balWatcher->deleteLater();
        auto *goalSumWatcher = new QFutureWatcher<qint64>(this);
        connect(goalSumWatcher, &QFutureWatcherBase::finished, this, [this, balance, goalSumWatcher, gen]() {
            if (gen == m_refreshGen.load()) {
                m_freeCash = GoalAllocator::freeCashForAllocation(balance, goalSumWatcher->result());
                emit freeCashChanged();
            }
            goalSumWatcher->deleteLater();
        });
        goalSumWatcher->setFuture(DatabaseManager::instance().totalGoalsCurrent());
    });
    balWatcher->setFuture(DatabaseManager::instance().totalAccountsBalance());
}

void GoalViewModel::addGoal(const QString &name, const QString &targetText, const QString &currentText,
                            const QString &deadline, bool autoAllocate)
{
    auto &fmt = MoneyFormatter::instance();
    const qint64 target = fmt.parseToCents(targetText);
    const qint64 current = fmt.parseToCents(currentText);
    if (name.trimmed().isEmpty() || target <= 0) {
        emit operationFinished(false, QStringLiteral("Укажите название и целевую сумму"));
        return;
    }
    auto *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result() > 0, QStringLiteral("Цель создана"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().addGoal(
        name.trimmed(), target, qMax<qint64>(0, current), deadline, autoAllocate));
}

void GoalViewModel::contribute(int goalId, const QString &addText)
{
    const qint64 addCents = MoneyFormatter::instance().parseToCents(addText);
    if (goalId <= 0 || addCents <= 0) {
        emit operationFinished(false, QStringLiteral("Укажите сумму пополнения"));
        return;
    }
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result(), QStringLiteral("Цель пополнена"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().contributeToGoal(goalId, addCents));
}

void GoalViewModel::autoAllocatePercent(int percent)
{
    const int p = qBound(0, percent, 100);
    const qint64 amount = (m_freeCash * p) / 100;
    if (amount <= 0) {
        emit operationFinished(false, QStringLiteral("Нет свободных средств"));
        return;
    }
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result(), QStringLiteral("Средства распределены"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().allocateToGoals(amount));
}
