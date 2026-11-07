#pragma once

#include <QAbstractListModel>
#include <QVariantList>

class MortgageScheduleModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        MonthIndexRole = Qt::UserRole + 1, PaymentCentsRole, InterestCentsRole,
        PrincipalCentsRole, RemainingCentsRole
    };
    explicit MortgageScheduleModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setItems(const QVariantList &items);
    QVariantList items() const { return m_items; }
private:
    QVariantList m_items;
};
