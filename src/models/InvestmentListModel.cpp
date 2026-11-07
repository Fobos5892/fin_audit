#include "InvestmentListModel.h"

InvestmentListModel::InvestmentListModel(QObject *parent) : QAbstractListModel(parent) {}

int InvestmentListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant InvestmentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case IdRole: return row.value(QStringLiteral("id"));
    case TickerRole: return row.value(QStringLiteral("ticker"));
    case NameRole: return row.value(QStringLiteral("name"));
    case QuantityRole: return row.value(QStringLiteral("quantity"));
    case AvgPriceCentsRole: return row.value(QStringLiteral("avgPriceCents"));
    case CurrentPriceCentsRole: return row.value(QStringLiteral("currentPriceCents"));
    case TypeRole: return row.value(QStringLiteral("type"));
    case ValueCentsRole: return row.value(QStringLiteral("valueCents"));
    case PnlCentsRole: return row.value(QStringLiteral("pnlCents"));
    case PnlPercentRole: return row.value(QStringLiteral("pnlPercent"));
    default: return {};
    }
}

QHash<int, QByteArray> InvestmentListModel::roleNames() const
{
    return {{IdRole, "id"}, {TickerRole, "ticker"}, {NameRole, "name"},
            {QuantityRole, "quantity"}, {AvgPriceCentsRole, "avgPriceCents"},
            {CurrentPriceCentsRole, "currentPriceCents"}, {TypeRole, "type"},
            {ValueCentsRole, "valueCents"}, {PnlCentsRole, "pnlCents"},
            {PnlPercentRole, "pnlPercent"}};
}

void InvestmentListModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}
