#include "GoalAllocator.h"

qint64 GoalAllocator::freeCashForAllocation(qint64 accountsBalance, qint64 reservedGoals, qint64 bufferCents)
{
    return qMax<qint64>(0, accountsBalance - reservedGoals - bufferCents);
}
