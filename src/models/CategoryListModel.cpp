#include "CategoryListModel.h"

CategoryListModel::CategoryListModel(QObject *parent) : QAbstractListModel(parent) {}

int CategoryListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant CategoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case IdRole: return row.value(QStringLiteral("id"));
    case NameRole: return row.value(QStringLiteral("name"));
    case TypeRole: return row.value(QStringLiteral("type"));
    case ParentIdRole: return row.value(QStringLiteral("parentId"));
    case IconRole: return row.value(QStringLiteral("icon"));
    case ChartColorRole: return row.value(QStringLiteral("chartColor"));
    default: return {};
    }
}

QHash<int, QByteArray> CategoryListModel::roleNames() const
{
    return {{IdRole, "id"}, {NameRole, "name"}, {TypeRole, "type"},
            {ParentIdRole, "parentId"}, {IconRole, "icon"}, {ChartColorRole, "chartColor"}};
}

void CategoryListModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
    emit countChanged();
}
