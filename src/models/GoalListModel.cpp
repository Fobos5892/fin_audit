#include "GoalListModel.h"

GoalListModel::GoalListModel(QObject *parent) : QAbstractListModel(parent) {}

int GoalListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant GoalListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case IdRole: return row.value(QStringLiteral("id"));
    case NameRole: return row.value(QStringLiteral("name"));
    case TargetCentsRole: return row.value(QStringLiteral("targetCents"));
    case CurrentCentsRole: return row.value(QStringLiteral("currentCents"));
    case DeadlineRole: return row.value(QStringLiteral("deadline"));
    case AutoAllocateRole: return row.value(QStringLiteral("autoAllocate"));
    case ProgressRole: return row.value(QStringLiteral("progress"));
    default: return {};
    }
}

QHash<int, QByteArray> GoalListModel::roleNames() const
{
    return {{IdRole, "id"}, {NameRole, "name"}, {TargetCentsRole, "targetCents"},
            {CurrentCentsRole, "currentCents"}, {DeadlineRole, "deadline"},
            {AutoAllocateRole, "autoAllocate"}, {ProgressRole, "progress"}};
}

void GoalListModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}
