#pragma once

#include <QObject>
#include <atomic>

#include "models/InvestmentListModel.h"

class InvestmentViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(InvestmentListModel *positions READ positions CONSTANT)
    Q_PROPERTY(qint64 portfolioValueCents READ portfolioValueCents NOTIFY summaryChanged)
    Q_PROPERTY(qint64 pnlCents READ pnlCents NOTIFY summaryChanged)
    Q_PROPERTY(double pnlPercent READ pnlPercent NOTIFY summaryChanged)

public:
    explicit InvestmentViewModel(QObject *parent = nullptr);

    InvestmentListModel *positions() const { return m_positions; }
    qint64 portfolioValueCents() const { return m_value; }
    qint64 pnlCents() const { return m_pnl; }
    double pnlPercent() const { return m_pnlPercent; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void trade(const QString &ticker, const QString &name, const QString &type,
                           bool isBuy, const QString &quantityText, const QString &priceText);
    Q_INVOKABLE void updatePrice(int id, const QString &priceText);

signals:
    void summaryChanged();
    void operationFinished(bool ok, const QString &message);

private:
    InvestmentListModel *m_positions = nullptr;
    qint64 m_value = 0;
    qint64 m_pnl = 0;
    double m_pnlPercent = 0.0;
    std::atomic<quint64> m_refreshGen{0};
};
