#include "AccountListModel.h"

AccountListModel::AccountListModel(QObject *parent) : QAbstractListModel(parent) {}

int AccountListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant AccountListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case IdRole: return row.value(QStringLiteral("id"));
    case NameRole: return row.value(QStringLiteral("name"));
    case TypeRole: return row.value(QStringLiteral("type"));
    case BalanceCentsRole: return row.value(QStringLiteral("balanceCents"));
    case CurrencyRole: return row.value(QStringLiteral("currency"));
    default: return {};
    }
}

QHash<int, QByteArray> AccountListModel::roleNames() const
{
    return {{IdRole, "id"}, {NameRole, "name"}, {TypeRole, "type"},
            {BalanceCentsRole, "balanceCents"}, {CurrencyRole, "currency"}};
}

void AccountListModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}
