#include "MortgageScheduleModel.h"

MortgageScheduleModel::MortgageScheduleModel(QObject *parent) : QAbstractListModel(parent) {}

int MortgageScheduleModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QVariant MortgageScheduleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto row = m_items.at(index.row()).toMap();
    switch (role) {
    case MonthIndexRole: return row.value(QStringLiteral("monthIndex"));
    case PaymentCentsRole: return row.value(QStringLiteral("paymentCents"));
    case InterestCentsRole: return row.value(QStringLiteral("interestCents"));
    case PrincipalCentsRole: return row.value(QStringLiteral("principalCents"));
    case RemainingCentsRole: return row.value(QStringLiteral("remainingCents"));
    default: return {};
    }
}

QHash<int, QByteArray> MortgageScheduleModel::roleNames() const
{
    return {{MonthIndexRole, "monthIndex"}, {PaymentCentsRole, "paymentCents"},
            {InterestCentsRole, "interestCents"}, {PrincipalCentsRole, "principalCents"},
            {RemainingCentsRole, "remainingCents"}};
}

void MortgageScheduleModel::setItems(const QVariantList &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}
