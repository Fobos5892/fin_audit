#pragma once

#include <QAbstractListModel>
#include <QVariantList>

class ChartSliceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(qint64 totalCents READ totalCents NOTIFY totalChanged)
public:
    enum Roles { CategoryIdRole = Qt::UserRole + 1, LabelRole, ColorRole, ValueCentsRole, RatioRole };
    explicit ChartSliceModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setItems(const QVariantList &items);
    qint64 totalCents() const { return m_total; }
    Q_INVOKABLE QVariantList asList() const;
signals:
    void totalChanged();
private:
    QVariantList m_items;
    qint64 m_total = 0;
};
