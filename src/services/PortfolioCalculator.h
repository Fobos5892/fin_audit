#pragma once

#include <QVariantList>
#include <QtGlobal>

struct PortfolioSummary {
    qint64 valueCents = 0;
    qint64 costCents = 0;
    qint64 pnlCents = 0;
    double pnlPercent = 0.0;
};

class PortfolioCalculator
{
public:
    static PortfolioSummary summarize(const QVariantList &positions);
};
