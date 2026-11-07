#pragma once

#include <QtGlobal>

class GoalAllocator
{
public:
    static qint64 freeCashForAllocation(qint64 accountsBalance, qint64 reservedGoals, qint64 bufferCents = 0);
};
