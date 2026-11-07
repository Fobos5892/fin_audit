#include "MoneyFormatter.h"

#include <QLocale>
#include <cmath>

MoneyFormatter &MoneyFormatter::instance()
{
    static MoneyFormatter inst;
    return inst;
}

MoneyFormatter::MoneyFormatter(QObject *parent)
    : QObject(parent)
{
}

QString MoneyFormatter::formatCents(qint64 cents, const QString &currency) const
{
    const QLocale locale(QLocale::Russian, QLocale::Russia);
    const double value = static_cast<double>(cents) / 100.0;
    return locale.toString(value, 'f', 2) + QLatin1Char(' ') + currency;
}

QString MoneyFormatter::formatCentsSigned(qint64 cents, const QString &currency) const
{
    const QString sign = cents > 0 ? QStringLiteral("+") : QString();
    return sign + formatCents(cents, currency);
}

qint64 MoneyFormatter::parseToCents(const QString &text) const
{
    QString cleaned = text;
    cleaned.replace(QStringLiteral("₽"), QString());
    cleaned.replace(QLatin1Char(' '), QString());
    cleaned.replace(QLatin1Char(','), QLatin1Char('.'));
    bool ok = false;
    const double value = cleaned.toDouble(&ok);
    if (!ok)
        return 0;
    return doubleToCents(value);
}

qint64 MoneyFormatter::doubleToCents(double value) const
{
    return static_cast<qint64>(std::llround(value * 100.0));
}

QString MoneyFormatter::ownershipLabel(const QString &code) const
{
    if (code.compare(QStringLiteral("personal"), Qt::CaseInsensitive) == 0)
        return QString::fromUtf8(u8"Личное");
    if (code.compare(QStringLiteral("family"), Qt::CaseInsensitive) == 0)
        return QString::fromUtf8(u8"Семейное");
    return code;
}

QString MoneyFormatter::ownershipCodeFromIndex(int index) const
{
    return index == 1 ? QStringLiteral("personal") : QStringLiteral("family");
}
