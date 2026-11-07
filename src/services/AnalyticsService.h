#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class AnalyticsService : public QObject
{
    Q_OBJECT
public:
    explicit AnalyticsService(QObject *parent = nullptr);

    Q_INVOKABLE void requestMonthAnalytics(int year, int month);
    Q_INVOKABLE void requestNetWorth();

signals:
    void monthAnalyticsReady(const QVariantMap &totals, const QVariantList &expenseSlices,
                             const QVariantList &incomeSlices);
    void netWorthReady(qint64 netWorthCents, const QVariantMap &parts);
};
