#include "InvestmentViewModel.h"

#include "core/MoneyFormatter.h"
#include "database/DatabaseManager.h"
#include "services/PortfolioCalculator.h"

#include <QDateTime>
#include <QFutureWatcher>

InvestmentViewModel::InvestmentViewModel(QObject *parent)
    : QObject(parent)
    , m_positions(new InvestmentListModel(this))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &InvestmentViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &InvestmentViewModel::refresh);
    if (DatabaseManager::instance().isReady())
        refresh();
}

void InvestmentViewModel::refresh()
{
    const quint64 gen = ++m_refreshGen;
    auto *watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, gen]() {
        if (gen == m_refreshGen.load()) {
            const auto items = watcher->result();
            m_positions->setItems(items);
            const auto summary = PortfolioCalculator::summarize(items);
            m_value = summary.valueCents;
            m_pnl = summary.pnlCents;
            m_pnlPercent = summary.pnlPercent;
            emit summaryChanged();
        }
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().fetchInvestments());
}

void InvestmentViewModel::trade(const QString &ticker, const QString &name, const QString &type,
                                bool isBuy, const QString &quantityText, const QString &priceText)
{
    bool qtyOk = false;
    const double quantity = quantityText.trimmed().replace(QLatin1Char(','), QLatin1Char('.')).toDouble(&qtyOk);
    const qint64 priceCents = MoneyFormatter::instance().parseToCents(priceText);
    if (ticker.trimmed().isEmpty() || !qtyOk || quantity <= 0.0 || priceCents <= 0) {
        emit operationFinished(false, QStringLiteral("Проверьте тикер, количество и цену"));
        return;
    }
    auto *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result() > 0, QStringLiteral("Сделка сохранена"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().upsertInvestmentTrade(
        ticker.trimmed().toUpper(), name.isEmpty() ? ticker : name, type,
        isBuy ? QStringLiteral("buy") : QStringLiteral("sell"), quantity, priceCents,
        QDateTime::currentDateTime().toString(Qt::ISODate)));
}

void InvestmentViewModel::updatePrice(int id, const QString &priceText)
{
    const qint64 priceCents = MoneyFormatter::instance().parseToCents(priceText);
    if (id <= 0 || priceCents <= 0) {
        emit operationFinished(false, QStringLiteral("Некорректная цена"));
        return;
    }
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        emit operationFinished(watcher->result(), QStringLiteral("Цена обновлена"));
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().updateInvestmentPrice(id, priceCents));
}
