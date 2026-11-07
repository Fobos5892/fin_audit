#pragma once

#include <QObject>
#include <QString>
#include <QtGlobal>

class MoneyFormatter : public QObject
{
    Q_OBJECT

public:
    static MoneyFormatter &instance();

    Q_INVOKABLE QString formatCents(qint64 cents, const QString &currency = QStringLiteral("₽")) const;
    Q_INVOKABLE QString formatCentsSigned(qint64 cents, const QString &currency = QStringLiteral("₽")) const;
    Q_INVOKABLE qint64 parseToCents(const QString &text) const;
    Q_INVOKABLE double centsToDouble(qint64 cents) const { return static_cast<double>(cents) / 100.0; }
    Q_INVOKABLE qint64 doubleToCents(double value) const;
    /// family / personal → «Семейное» / «Личное»
    Q_INVOKABLE QString ownershipLabel(const QString &code) const;
    /// индекс в UI-списке [Семейное, Личное] → код для БД
    Q_INVOKABLE QString ownershipCodeFromIndex(int index) const;

private:
    explicit MoneyFormatter(QObject *parent = nullptr);
};
