#include "services/MortgageCalculator.h"

#include <QtTest>

class TstMortgageCalculator : public QObject
{
    Q_OBJECT
private slots:
    void annuity_basicPayment();
    void extraPayment_reducesTermAndInterest();
    void zeroRate_dividesEvenly();
};

void TstMortgageCalculator::annuity_basicPayment()
{
    MortgageInput in;
    in.principalCents = 1'000'000'00; // 1_000_000 rub
    in.rateBps = 1200;                // 12%
    in.termMonths = 12;
    const auto r = MortgageCalculator::calculate(in, false);
    QVERIFY(r.monthlyPaymentCents > 0);
    QCOMPARE(r.months, 12);
    QVERIFY(r.totalInterestCents > 0);
    QVERIFY(!r.schedule.isEmpty());
}

void TstMortgageCalculator::extraPayment_reducesTermAndInterest()
{
    MortgageInput in;
    in.principalCents = 5'000'000'00;
    in.rateBps = 1000;
    in.termMonths = 240;
    in.extraPaymentCents = 50'000'00;
    const auto base = MortgageCalculator::calculate(in, false);
    const auto extra = MortgageCalculator::calculate(in, true);
    QVERIFY(extra.months < base.months);
    QVERIFY(extra.totalInterestCents < base.totalInterestCents);
}

void TstMortgageCalculator::zeroRate_dividesEvenly()
{
    MortgageInput in;
    in.principalCents = 120'000;
    in.rateBps = 0;
    in.termMonths = 12;
    const auto r = MortgageCalculator::calculate(in, false);
    QCOMPARE(r.monthlyPaymentCents, 10000LL);
    QCOMPARE(r.totalInterestCents, 0LL);
}

QTEST_MAIN(TstMortgageCalculator)
#include "tst_mortgagecalculator.moc"
