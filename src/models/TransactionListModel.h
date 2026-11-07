#pragma once

#include <QAbstractListModel>
#include <QVariantList>

class TransactionListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        AccountIdRole,
        CategoryIdRole,
        AmountCentsRole,
        DateRole,
        DescriptionRole,
        OwnershipRole,
        IsInvestmentRole,
        CategoryNameRole,
        ChartColorRole,
        AccountNameRole
    };

    explicit TransactionListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItems(const QVariantList &items);

private:
    QVariantList m_items;
};
