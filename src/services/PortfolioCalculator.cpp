#include "PortfolioCalculator.h"

#include <QVariantMap>

PortfolioSummary PortfolioCalculator::summarize(const QVariantList &positions)
{
    PortfolioSummary s;
    for (const auto &v : positions) {
        const auto m = v.toMap();
        s.valueCents += m.value(QStringLiteral("valueCents")).toLongLong();
        const double qty = m.value(QStringLiteral("quantity")).toDouble();
        const qint64 avg = m.value(QStringLiteral("avgPriceCents")).toLongLong();
        s.costCents += static_cast<qint64>(qty * static_cast<double>(avg));
    }
    s.pnlCents = s.valueCents - s.costCents;
    s.pnlPercent = s.costCents > 0
        ? 100.0 * static_cast<double>(s.pnlCents) / static_cast<double>(s.costCents)
        : 0.0;
    return s;
}
