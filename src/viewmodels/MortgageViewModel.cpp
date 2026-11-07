#include "MortgageViewModel.h"

#include "core/MoneyFormatter.h"
#include "database/DatabaseManager.h"
#include "services/MortgageCalculator.h"

#include <QDate>
#include <QFutureWatcher>
#include <cmath>

MortgageViewModel::MortgageViewModel(QObject *parent)
    : QObject(parent)
    , m_scheduleBase(new MortgageScheduleModel(this))
    , m_scheduleExtra(new MortgageScheduleModel(this))
    , m_startDate(QDate::currentDate().toString(Qt::ISODate))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &MortgageViewModel::refresh);
    if (DatabaseManager::instance().isReady())
        refresh();
    else
        recalculate();
}

void MortgageViewModel::setName(const QString &v)
{
    if (m_name == v)
        return;
    m_name = v;
    emit mortgageChanged();
}

void MortgageViewModel::setPrincipalRub(double v)
{
    if (qFuzzyCompare(m_principalRub, v))
        return;
    m_principalRub = v;
    emit mortgageChanged();
    recalculate();
}

void MortgageViewModel::setRatePercent(double v)
{
    if (qFuzzyCompare(m_ratePercent, v))
        return;
    m_ratePercent = v;
    emit mortgageChanged();
    recalculate();
}

void MortgageViewModel::setTermMonths(int v)
{
    if (m_termMonths == v)
        return;
    m_termMonths = v;
    emit mortgageChanged();
    recalculate();
}

void MortgageViewModel::setStartDate(const QString &v)
{
    if (m_startDate == v)
        return;
    m_startDate = v;
    emit mortgageChanged();
}

void MortgageViewModel::setExtraPaymentRub(double v)
{
    if (qFuzzyCompare(m_extraRub, v))
        return;
    m_extraRub = v;
    emit mortgageChanged();
    recalculate();
}

void MortgageViewModel::loadFromMap(const QVariantMap &map)
{
    m_id = map.value(QStringLiteral("id")).toInt();
    m_name = map.value(QStringLiteral("name"), m_name).toString();
    m_principalRub = MoneyFormatter::instance().centsToDouble(
        map.value(QStringLiteral("principalCents")).toLongLong());
    m_ratePercent = map.value(QStringLiteral("rateBps")).toInt() / 100.0;
    m_termMonths = map.value(QStringLiteral("termMonths")).toInt();
    m_startDate = map.value(QStringLiteral("startDate")).toString();
    m_extraRub = MoneyFormatter::instance().centsToDouble(
        map.value(QStringLiteral("extraPaymentCents")).toLongLong());
    emit mortgageChanged();
    recalculate();
}

void MortgageViewModel::refresh()
{
    auto *watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        const auto list = watcher->result();
        if (!list.isEmpty())
            loadFromMap(list.first().toMap());
        else
            recalculate();
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().fetchMortgages());
}

void MortgageViewModel::recalculate()
{
    MortgageInput input;
    input.principalCents = MoneyFormatter::instance().doubleToCents(m_principalRub);
    input.rateBps = static_cast<int>(std::llround(m_ratePercent * 100.0));
    input.termMonths = m_termMonths;
    input.extraPaymentCents = MoneyFormatter::instance().doubleToCents(m_extraRub);

    const auto base = MortgageCalculator::calculate(input, false);
    const auto extra = MortgageCalculator::calculate(input, true);

    m_monthlyPayment = base.monthlyPaymentCents;
    m_interestBase = base.totalInterestCents;
    m_interestExtra = extra.totalInterestCents;
    m_monthsBase = base.months;
    m_monthsExtra = extra.months;
    m_scheduleBase->setItems(base.schedule);
    m_scheduleExtra->setItems(extra.schedule);
    rebuildChartSeries();
    emit calcChanged();
}

void MortgageViewModel::rebuildChartSeries()
{
    m_remainingBase.clear();
    m_remainingExtra.clear();
    m_interestSeriesBase.clear();
    m_interestSeriesExtra.clear();

    auto build = [](const QVariantList &schedule, QVariantList &remainingOut, QVariantList &interestOut) {
        qint64 cumulativeInterest = 0;
        for (const QVariant &v : schedule) {
            const QVariantMap row = v.toMap();
            const int month = row.value(QStringLiteral("monthIndex")).toInt();
            cumulativeInterest += row.value(QStringLiteral("interestCents")).toLongLong();

            QVariantMap rem;
            rem.insert(QStringLiteral("month"), month);
            rem.insert(QStringLiteral("valueCents"), row.value(QStringLiteral("remainingCents")).toLongLong());
            remainingOut.append(rem);

            QVariantMap interest;
            interest.insert(QStringLiteral("month"), month);
            interest.insert(QStringLiteral("valueCents"), cumulativeInterest);
            interestOut.append(interest);
        }
    };

    build(m_scheduleBase->items(), m_remainingBase, m_interestSeriesBase);
    build(m_scheduleExtra->items(), m_remainingExtra, m_interestSeriesExtra);
}

void MortgageViewModel::save()
{
    recalculate();
    const qint64 principalCents = MoneyFormatter::instance().doubleToCents(m_principalRub);
    QVariantMap data;
    if (m_id > 0)
        data.insert(QStringLiteral("id"), m_id);
    data.insert(QStringLiteral("name"), m_name);
    data.insert(QStringLiteral("principalCents"), principalCents);
    data.insert(QStringLiteral("rateBps"), static_cast<int>(std::llround(m_ratePercent * 100.0)));
    data.insert(QStringLiteral("termMonths"), m_termMonths);
    data.insert(QStringLiteral("startDate"), m_startDate);
    data.insert(QStringLiteral("extraPaymentCents"), MoneyFormatter::instance().doubleToCents(m_extraRub));
    data.insert(QStringLiteral("remainingCents"), principalCents);

    const QVariantList baseRows = m_scheduleBase->items();
    const QVariantList extraRows = m_scheduleExtra->items();

    auto *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, baseRows, extraRows]() {
        const int id = watcher->result();
        watcher->deleteLater();
        if (id <= 0) {
            emit operationFinished(false, QStringLiteral("Ошибка сохранения"));
            return;
        }
        m_id = id;
        emit mortgageChanged();

        auto *baseWatcher = new QFutureWatcher<bool>(this);
        connect(baseWatcher, &QFutureWatcherBase::finished, this, [this, baseWatcher, id, extraRows]() {
            const bool baseOk = baseWatcher->result();
            baseWatcher->deleteLater();
            auto *extraWatcher = new QFutureWatcher<bool>(this);
            connect(extraWatcher, &QFutureWatcherBase::finished, this, [this, extraWatcher, baseOk]() {
                const bool ok = baseOk && extraWatcher->result();
                emit operationFinished(ok, ok ? QStringLiteral("Ипотека сохранена")
                                              : QStringLiteral("Ошибка графика"));
                extraWatcher->deleteLater();
            });
            extraWatcher->setFuture(DatabaseManager::instance().replaceMortgageSchedule(id, extraRows, true));
        });
        baseWatcher->setFuture(DatabaseManager::instance().replaceMortgageSchedule(id, baseRows, false));
    });
    watcher->setFuture(DatabaseManager::instance().saveMortgage(data));
}
