#pragma once

#include <QObject>
#include <atomic>

#include "models/GoalListModel.h"

class GoalViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GoalListModel *goals READ goals CONSTANT)
    Q_PROPERTY(qint64 freeCashCents READ freeCashCents NOTIFY freeCashChanged)

public:
    explicit GoalViewModel(QObject *parent = nullptr);

    GoalListModel *goals() const { return m_goals; }
    qint64 freeCashCents() const { return m_freeCash; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addGoal(const QString &name, const QString &targetText, const QString &currentText,
                             const QString &deadline, bool autoAllocate);
    Q_INVOKABLE void contribute(int goalId, const QString &addText);
    Q_INVOKABLE void autoAllocatePercent(int percent);

signals:
    void freeCashChanged();
    void operationFinished(bool ok, const QString &message);

private:
    GoalListModel *m_goals = nullptr;
    qint64 m_freeCash = 0;
    std::atomic<quint64> m_refreshGen{0};
};
