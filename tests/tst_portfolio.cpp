#include "services/PortfolioCalculator.h"

#include <QtTest>

class TstPortfolio : public QObject
{
    Q_OBJECT
private slots:
    void summarize_pnl();
};

void TstPortfolio::summarize_pnl()
{
    QVariantList positions;
    QVariantMap a;
    a.insert(QStringLiteral("quantity"), 10.0);
    a.insert(QStringLiteral("avgPriceCents"), 10000);
    a.insert(QStringLiteral("valueCents"), 120000);
    positions.append(a);

    const auto s = PortfolioCalculator::summarize(positions);
    QCOMPARE(s.valueCents, 120000LL);
    QCOMPARE(s.costCents, 100000LL);
    QCOMPARE(s.pnlCents, 20000LL);
    QVERIFY(qFuzzyCompare(s.pnlPercent, 20.0));
}

QTEST_MAIN(TstPortfolio)
#include "tst_portfolio.moc"
