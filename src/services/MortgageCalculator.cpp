#include "MortgageCalculator.h"

#include <QVariantMap>
#include <cmath>

MortgageResult MortgageCalculator::calculate(const MortgageInput &input, bool applyExtra)
{
    MortgageResult result;
    if (input.principalCents <= 0 || input.termMonths <= 0)
        return result;

    const double monthlyRate = (static_cast<double>(input.rateBps) / 10000.0) / 12.0;
    const int n = input.termMonths;
    double payment = 0.0;
    if (monthlyRate <= 0.0) {
        payment = static_cast<double>(input.principalCents) / n;
    } else {
        const double factor = std::pow(1.0 + monthlyRate, n);
        payment = static_cast<double>(input.principalCents) * monthlyRate * factor / (factor - 1.0);
    }

    result.monthlyPaymentCents = static_cast<qint64>(std::llround(payment));
    double balance = static_cast<double>(input.principalCents);
    qint64 totalInterest = 0;
    qint64 totalPaid = 0;
    int month = 0;

    while (balance > 1.0 && month < n + 600) {
        ++month;
        const double interest = balance * monthlyRate;
        double principalPart = payment - interest;
        if (applyExtra && input.extraPaymentCents > 0)
            principalPart += static_cast<double>(input.extraPaymentCents);

        if (principalPart > balance)
            principalPart = balance;

        const double pay = interest + principalPart;
        balance -= principalPart;
        totalInterest += static_cast<qint64>(std::llround(interest));
        totalPaid += static_cast<qint64>(std::llround(pay));

        QVariantMap row;
        row.insert(QStringLiteral("monthIndex"), month);
        row.insert(QStringLiteral("paymentCents"), static_cast<qint64>(std::llround(pay)));
        row.insert(QStringLiteral("interestCents"), static_cast<qint64>(std::llround(interest)));
        row.insert(QStringLiteral("principalCents"), static_cast<qint64>(std::llround(principalPart)));
        row.insert(QStringLiteral("remainingCents"), static_cast<qint64>(std::llround(qMax(0.0, balance))));
        result.schedule.append(row);

        if (balance <= 1.0)
            break;
    }

    result.totalInterestCents = totalInterest;
    result.totalPaidCents = totalPaid;
    result.months = month;
    return result;
}
