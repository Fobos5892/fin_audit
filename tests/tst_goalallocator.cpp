#include "services/GoalAllocator.h"

#include <QtTest>

class TstGoalAllocator : public QObject
{
    Q_OBJECT
private slots:
    void freeCash_basic();
};

void TstGoalAllocator::freeCash_basic()
{
    QCOMPARE(GoalAllocator::freeCashForAllocation(1000, 300, 100), 600LL);
    QCOMPARE(GoalAllocator::freeCashForAllocation(100, 500, 0), 0LL);
}

QTEST_MAIN(TstGoalAllocator)
#include "tst_goalallocator.moc"
