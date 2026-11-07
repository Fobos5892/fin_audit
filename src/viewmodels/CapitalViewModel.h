#pragma once

#include <QObject>
#include <QVariantList>

#include "models/MortgageScheduleModel.h"
#include "services/AnalyticsService.h"

class CapitalViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 accountsCents READ accountsCents NOTIFY capitalChanged)
    Q_PROPERTY(qint64 portfolioCents READ portfolioCents NOTIFY capitalChanged)
    Q_PROPERTY(qint64 mortgageCents READ mortgageCents NOTIFY capitalChanged)
    Q_PROPERTY(qint64 goalsCents READ goalsCents NOTIFY capitalChanged)
    Q_PROPERTY(qint64 netWorthCents READ netWorthCents NOTIFY capitalChanged)
    Q_PROPERTY(QVariantList projectionBase READ projectionBase NOTIFY capitalChanged)
    Q_PROPERTY(QVariantList projectionExtra READ projectionExtra NOTIFY capitalChanged)
    Q_PROPERTY(MortgageScheduleModel *mortgageBase READ mortgageBase CONSTANT)
    Q_PROPERTY(MortgageScheduleModel *mortgageExtra READ mortgageExtra CONSTANT)

public:
    explicit CapitalViewModel(QObject *parent = nullptr);

    qint64 accountsCents() const { return m_accounts; }
    qint64 portfolioCents() const { return m_portfolio; }
    qint64 mortgageCents() const { return m_mortgage; }
    qint64 goalsCents() const { return m_goals; }
    qint64 netWorthCents() const { return m_netWorth; }
    QVariantList projectionBase() const { return m_projectionBase; }
    QVariantList projectionExtra() const { return m_projectionExtra; }
    MortgageScheduleModel *mortgageBase() const { return m_mortgageBase; }
    MortgageScheduleModel *mortgageExtra() const { return m_mortgageExtra; }

    Q_INVOKABLE void refresh();

signals:
    void capitalChanged();

private:
    void rebuildProjections();

    AnalyticsService *m_analytics = nullptr;
    MortgageScheduleModel *m_mortgageBase = nullptr;
    MortgageScheduleModel *m_mortgageExtra = nullptr;
    qint64 m_accounts = 0;
    qint64 m_portfolio = 0;
    qint64 m_mortgage = 0;
    qint64 m_goals = 0;
    qint64 m_netWorth = 0;
    QVariantList m_projectionBase;
    QVariantList m_projectionExtra;
};
