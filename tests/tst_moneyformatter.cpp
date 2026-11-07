#include "core/MoneyFormatter.h"

#include <QtTest>

class TstMoneyFormatter : public QObject
{
    Q_OBJECT
private slots:
    void doubleToCents_rounds();
    void parseToCents_acceptsComma();
    void formatCents_containsCurrency();
};

void TstMoneyFormatter::doubleToCents_rounds()
{
    auto &fmt = MoneyFormatter::instance();
    QCOMPARE(fmt.doubleToCents(10.01), 1001LL);
    QCOMPARE(fmt.doubleToCents(12.34), 1234LL);
    QCOMPARE(fmt.centsToDouble(1234), 12.34);
}

void TstMoneyFormatter::parseToCents_acceptsComma()
{
    auto &fmt = MoneyFormatter::instance();
    QCOMPARE(fmt.parseToCents(QStringLiteral("1 234,56 ₽")), 123456LL);
    QCOMPARE(fmt.parseToCents(QStringLiteral("0")), 0LL);
    QCOMPARE(fmt.parseToCents(QStringLiteral("abc")), 0LL);
}

void TstMoneyFormatter::formatCents_containsCurrency()
{
    const QString s = MoneyFormatter::instance().formatCents(150075, QStringLiteral("₽"));
    QVERIFY(s.contains(QStringLiteral("₽")));
    QVERIFY(s.contains(QStringLiteral("1")));
}

QTEST_MAIN(TstMoneyFormatter)
#include "tst_moneyformatter.moc"
