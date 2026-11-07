#include "ChartSliceModel.h"

ChartSliceModel::ChartSliceModel(QObject *parent) : QAbstractListModel(parent) {}

int ChartSliceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant ChartSliceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case CategoryIdRole: return row.value(QStringLiteral("categoryId"));
    case LabelRole: return row.value(QStringLiteral("label"));
    case ColorRole: return row.value(QStringLiteral("color"));
    case ValueCentsRole: return row.value(QStringLiteral("valueCents"));
    case RatioRole: {
        const qint64 v = row.value(QStringLiteral("valueCents")).toLongLong();
        return m_total > 0 ? static_cast<double>(v) / static_cast<double>(m_total) : 0.0;
    }
    default: return {};
    }
}

QHash<int, QByteArray> ChartSliceModel::roleNames() const
{
    return {{CategoryIdRole, "categoryId"}, {LabelRole, "label"}, {ColorRole, "color"},
            {ValueCentsRole, "valueCents"}, {RatioRole, "ratio"}};
}

void ChartSliceModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    m_total = 0;
    for (const auto &v : m_items)
        m_total += v.toMap().value(QStringLiteral("valueCents")).toLongLong();
    endResetModel();
    emit totalChanged();
}

QVariantList ChartSliceModel::asList() const
{
    QVariantList out;
    for (const auto &v : m_items) {
        const auto m = v.toMap();
        QVariantMap slice;
        slice.insert(QStringLiteral("color"), m.value(QStringLiteral("color")));
        slice.insert(QStringLiteral("value"), m.value(QStringLiteral("valueCents")));
        slice.insert(QStringLiteral("label"), m.value(QStringLiteral("label")));
        out.append(slice);
    }
    return out;
}
