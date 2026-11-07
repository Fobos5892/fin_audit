#pragma once

#include <QObject>
#include <QVariantList>

#include "models/MortgageScheduleModel.h"

class MortgageViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int mortgageId READ mortgageId NOTIFY mortgageChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY mortgageChanged)
    Q_PROPERTY(double principalRub READ principalRub WRITE setPrincipalRub NOTIFY mortgageChanged)
    Q_PROPERTY(double ratePercent READ ratePercent WRITE setRatePercent NOTIFY mortgageChanged)
    Q_PROPERTY(int termMonths READ termMonths WRITE setTermMonths NOTIFY mortgageChanged)
    Q_PROPERTY(QString startDate READ startDate WRITE setStartDate NOTIFY mortgageChanged)
    Q_PROPERTY(double extraPaymentRub READ extraPaymentRub WRITE setExtraPaymentRub NOTIFY mortgageChanged)
    Q_PROPERTY(qint64 monthlyPaymentCents READ monthlyPaymentCents NOTIFY calcChanged)
    Q_PROPERTY(qint64 totalInterestCents READ totalInterestCents NOTIFY calcChanged)
    Q_PROPERTY(qint64 totalInterestWithExtraCents READ totalInterestWithExtraCents NOTIFY calcChanged)
    Q_PROPERTY(qint64 interestSavedCents READ interestSavedCents NOTIFY calcChanged)
    Q_PROPERTY(int monthsBase READ monthsBase NOTIFY calcChanged)
    Q_PROPERTY(int monthsWithExtra READ monthsWithExtra NOTIFY calcChanged)
    Q_PROPERTY(MortgageScheduleModel *scheduleBase READ scheduleBase CONSTANT)
    Q_PROPERTY(MortgageScheduleModel *scheduleExtra READ scheduleExtra CONSTANT)
    Q_PROPERTY(QVariantList remainingSeriesBase READ remainingSeriesBase NOTIFY calcChanged)
    Q_PROPERTY(QVariantList remainingSeriesExtra READ remainingSeriesExtra NOTIFY calcChanged)
    Q_PROPERTY(QVariantList interestSeriesBase READ interestSeriesBase NOTIFY calcChanged)
    Q_PROPERTY(QVariantList interestSeriesExtra READ interestSeriesExtra NOTIFY calcChanged)

public:
    explicit MortgageViewModel(QObject *parent = nullptr);

    int mortgageId() const { return m_id; }
    QString name() const { return m_name; }
    void setName(const QString &v);
    double principalRub() const { return m_principalRub; }
    void setPrincipalRub(double v);
    double ratePercent() const { return m_ratePercent; }
    void setRatePercent(double v);
    int termMonths() const { return m_termMonths; }
    void setTermMonths(int v);
    QString startDate() const { return m_startDate; }
    void setStartDate(const QString &v);
    double extraPaymentRub() const { return m_extraRub; }
    void setExtraPaymentRub(double v);

    qint64 monthlyPaymentCents() const { return m_monthlyPayment; }
    qint64 totalInterestCents() const { return m_interestBase; }
    qint64 totalInterestWithExtraCents() const { return m_interestExtra; }
    qint64 interestSavedCents() const { return qMax<qint64>(0, m_interestBase - m_interestExtra); }
    int monthsBase() const { return m_monthsBase; }
    int monthsWithExtra() const { return m_monthsExtra; }
    MortgageScheduleModel *scheduleBase() const { return m_scheduleBase; }
    MortgageScheduleModel *scheduleExtra() const { return m_scheduleExtra; }
    QVariantList remainingSeriesBase() const { return m_remainingBase; }
    QVariantList remainingSeriesExtra() const { return m_remainingExtra; }
    QVariantList interestSeriesBase() const { return m_interestSeriesBase; }
    QVariantList interestSeriesExtra() const { return m_interestSeriesExtra; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void recalculate();
    Q_INVOKABLE void save();

signals:
    void mortgageChanged();
    void calcChanged();
    void operationFinished(bool ok, const QString &message);

private:
    void loadFromMap(const QVariantMap &map);
    void rebuildChartSeries();

    MortgageScheduleModel *m_scheduleBase = nullptr;
    MortgageScheduleModel *m_scheduleExtra = nullptr;
    int m_id = -1;
    QString m_name = QStringLiteral("Ипотека");
    double m_principalRub = 5000000.0;
    double m_ratePercent = 12.0;
    int m_termMonths = 240;
    QString m_startDate;
    double m_extraRub = 0.0;
    qint64 m_monthlyPayment = 0;
    qint64 m_interestBase = 0;
    qint64 m_interestExtra = 0;
    int m_monthsBase = 0;
    int m_monthsExtra = 0;
    QVariantList m_remainingBase;
    QVariantList m_remainingExtra;
    QVariantList m_interestSeriesBase;
    QVariantList m_interestSeriesExtra;
};
