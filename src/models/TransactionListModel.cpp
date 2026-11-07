#include "TransactionListModel.h"

TransactionListModel::TransactionListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TransactionListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant TransactionListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};
    const QVariantMap row = m_items.at(index.row()).toMap();
    switch (role) {
    case IdRole: return row.value(QStringLiteral("id"));
    case AccountIdRole: return row.value(QStringLiteral("accountId"));
    case CategoryIdRole: return row.value(QStringLiteral("categoryId"));
    case AmountCentsRole: return row.value(QStringLiteral("amountCents"));
    case DateRole: return row.value(QStringLiteral("date"));
    case DescriptionRole: return row.value(QStringLiteral("description"));
    case OwnershipRole: return row.value(QStringLiteral("ownership"));
    case IsInvestmentRole: return row.value(QStringLiteral("isInvestment"));
    case CategoryNameRole: return row.value(QStringLiteral("categoryName"));
    case ChartColorRole: return row.value(QStringLiteral("chartColor"));
    case AccountNameRole: return row.value(QStringLiteral("accountName"));
    default: return {};
    }
}

QHash<int, QByteArray> TransactionListModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {AccountIdRole, "accountId"},
        {CategoryIdRole, "categoryId"},
        {AmountCentsRole, "amountCents"},
        {DateRole, "date"},
        {DescriptionRole, "description"},
        {OwnershipRole, "ownership"},
        {IsInvestmentRole, "isInvestment"},
        {CategoryNameRole, "categoryName"},
        {ChartColorRole, "chartColor"},
        {AccountNameRole, "accountName"},
    };
}

void TransactionListModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}
