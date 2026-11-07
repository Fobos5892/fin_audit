#include "CapitalViewModel.h"

#include "core/MoneyFormatter.h"
#include "database/DatabaseManager.h"
#include "services/MortgageCalculator.h"

#include <QFutureWatcher>
#include <QVariantMap>

CapitalViewModel::CapitalViewModel(QObject *parent)
    : QObject(parent)
    , m_analytics(new AnalyticsService(this))
    , m_mortgageBase(new MortgageScheduleModel(this))
    , m_mortgageExtra(new MortgageScheduleModel(this))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &CapitalViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &CapitalViewModel::refresh);
    connect(m_analytics, &AnalyticsService::netWorthReady, this,
            [this](qint64 net, const QVariantMap &parts) {
                m_netWorth = net;
                m_accounts = parts.value(QStringLiteral("accountsCents")).toLongLong();
                m_portfolio = parts.value(QStringLiteral("portfolioCents")).toLongLong();
                m_mortgage = parts.value(QStringLiteral("mortgageCents")).toLongLong();
                m_goals = parts.value(QStringLiteral("goalsCents")).toLongLong();
                rebuildProjections();
                emit capitalChanged();
            });
    if (DatabaseManager::instance().isReady())
        refresh();
}

void CapitalViewModel::refresh()
{
    m_analytics->requestNetWorth();

    auto *watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        const auto list = watcher->result();
        if (!list.isEmpty()) {
            const auto m = list.first().toMap();
            MortgageInput input;
            input.principalCents = m.value(QStringLiteral("principalCents")).toLongLong();
            input.rateBps = m.value(QStringLiteral("rateBps")).toInt();
            input.termMonths = m.value(QStringLiteral("termMonths")).toInt();
            input.extraPaymentCents = m.value(QStringLiteral("extraPaymentCents")).toLongLong();
            const auto base = MortgageCalculator::calculate(input, false);
            const auto extra = MortgageCalculator::calculate(input, true);
            m_mortgageBase->setItems(base.schedule);
            m_mortgageExtra->setItems(extra.schedule);
            rebuildProjections();
            emit capitalChanged();
        }
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().fetchMortgages());
}

void CapitalViewModel::rebuildProjections()
{
    m_projectionBase.clear();
    m_projectionExtra.clear();

    const qint64 liquid = m_accounts + m_portfolio;
    const auto baseItems = m_mortgageBase->items();
    const auto extraItems = m_mortgageExtra->items();

    auto pushSeries = [&](QVariantList &out, const QVariantList &schedule) {
        if (schedule.isEmpty()) {
            QVariantMap p0;
            p0.insert(QStringLiteral("month"), 0);
            p0.insert(QStringLiteral("valueCents"), liquid);
            out.append(p0);
            return;
        }
        for (const auto &v : schedule) {
            const auto row = v.toMap();
            const int month = row.value(QStringLiteral("monthIndex")).toInt();
            const qint64 remaining = row.value(QStringLiteral("remainingCents")).toLongLong();
            QVariantMap p;
            p.insert(QStringLiteral("month"), month);
            p.insert(QStringLiteral("valueCents"), liquid - remaining);
            out.append(p);
            if (month >= 120)
                break;
        }
    };

    pushSeries(m_projectionBase, baseItems);
    pushSeries(m_projectionExtra, extraItems);
}
