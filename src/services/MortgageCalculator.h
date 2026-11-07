#pragma once

#include <QVariantList>
#include <QtGlobal>

struct MortgageInput {
    qint64 principalCents = 0;
    int rateBps = 0; // 1200 = 12.00%
    int termMonths = 0;
    qint64 extraPaymentCents = 0;
};

struct MortgageResult {
    qint64 monthlyPaymentCents = 0;
    qint64 totalInterestCents = 0;
    qint64 totalPaidCents = 0;
    int months = 0;
    QVariantList schedule;
};

class MortgageCalculator
{
public:
    static MortgageResult calculate(const MortgageInput &input, bool applyExtra);
};
