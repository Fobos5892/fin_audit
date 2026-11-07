#pragma once

#include <QAbstractListModel>
#include <QVariantList>

class CategoryListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles { IdRole = Qt::UserRole + 1, NameRole, TypeRole, ParentIdRole, IconRole, ChartColorRole };
    explicit CategoryListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setItems(const QVariantList &items);
signals:
    void countChanged();
private:
    QVariantList m_items;
};
