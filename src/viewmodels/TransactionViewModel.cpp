#include "TransactionViewModel.h"

#include "core/MoneyFormatter.h"
#include "database/DatabaseManager.h"

#include <QDateTime>
#include <QFutureWatcher>

TransactionViewModel::TransactionViewModel(QObject *parent)
    : QObject(parent)
    , m_transactions(new TransactionListModel(this))
    , m_incomeTransactions(new TransactionListModel(this))
    , m_expenseTransactions(new TransactionListModel(this))
    , m_accounts(new AccountListModel(this))
    , m_categories(new CategoryListModel(this))
    , m_incomeCategories(new CategoryListModel(this))
    , m_expenseCategories(new CategoryListModel(this))
{
    connect(&DatabaseManager::instance(), &DatabaseManager::dataChanged, this, &TransactionViewModel::refresh);
    connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, this, &TransactionViewModel::refresh);
    if (DatabaseManager::instance().isReady())
        refresh();
}

void TransactionViewModel::setFilterOwnership(const QString &v)
{
    if (m_filterOwnership == v)
        return;
    m_filterOwnership = v;
    applyFilter(m_allTransactions);
    emit filterChanged();
}

void TransactionViewModel::applyFilter(const QVariantList &all)
{
    QVariantList filtered;
    if (m_filterOwnership.isEmpty()) {
        filtered = all;
    } else {
        for (const auto &v : all) {
            if (v.toMap().value(QStringLiteral("ownership")).toString() == m_filterOwnership)
                filtered.append(v);
        }
    }

    QVariantList income;
    QVariantList expense;
    for (const auto &v : filtered) {
        if (v.toMap().value(QStringLiteral("amountCents")).toLongLong() >= 0)
            income.append(v);
        else
            expense.append(v);
    }
    m_transactions->setItems(filtered);
    m_incomeTransactions->setItems(income);
    m_expenseTransactions->setItems(expense);
}

void TransactionViewModel::splitCategories(const QVariantList &all)
{
    QVariantList income;
    QVariantList expense;
    for (const auto &v : all) {
        const QString type = v.toMap().value(QStringLiteral("type")).toString();
        if (type == QLatin1String("income"))
            income.append(v);
        else
            expense.append(v);
    }
    m_categories->setItems(all);
    m_incomeCategories->setItems(income);
    m_expenseCategories->setItems(expense);
}

int TransactionViewModel::accountIdAt(int row) const
{
    return m_accounts->data(m_accounts->index(row), AccountListModel::IdRole).toInt();
}

int TransactionViewModel::idAt(CategoryListModel *model, int row)
{
    if (!model || row < 0 || row >= model->rowCount())
        return -1;
    return model->data(model->index(row), CategoryListModel::IdRole).toInt();
}

int TransactionViewModel::categoryIdAt(int row) const
{
    return idAt(m_categories, row);
}

int TransactionViewModel::incomeCategoryIdAt(int row) const
{
    return idAt(m_incomeCategories, row);
}

int TransactionViewModel::expenseCategoryIdAt(int row) const
{
    return idAt(m_expenseCategories, row);
}

void TransactionViewModel::refresh()
{
    const quint64 gen = ++m_refreshGen;

    auto *tx = new QFutureWatcher<QVariantList>(this);
    connect(tx, &QFutureWatcherBase::finished, this, [this, tx, gen]() {
        if (gen == m_refreshGen.load()) {
            m_allTransactions = tx->result();
            applyFilter(m_allTransactions);
        }
        tx->deleteLater();
    });
    tx->setFuture(DatabaseManager::instance().fetchTransactions(500));

    auto *acc = new QFutureWatcher<QVariantList>(this);
    connect(acc, &QFutureWatcherBase::finished, this, [this, acc, gen]() {
        if (gen == m_refreshGen.load())
            m_accounts->setItems(acc->result());
        acc->deleteLater();
    });
    acc->setFuture(DatabaseManager::instance().fetchAccounts());

    auto *cat = new QFutureWatcher<QVariantList>(this);
    connect(cat, &QFutureWatcherBase::finished, this, [this, cat, gen]() {
        if (gen == m_refreshGen.load())
            splitCategories(cat->result());
        cat->deleteLater();
    });
    cat->setFuture(DatabaseManager::instance().fetchCategories());
}

void TransactionViewModel::submitTransaction(int accountIndex, int categoryIndex, const QString &amountText,
                                             const QString &description, const QString &ownership, bool isIncome)
{
    if (accountIndex < 0 || accountIndex >= m_accounts->rowCount()) {
        m_lastError = QStringLiteral("Выберите счёт");
        emit operationFinished(false, m_lastError);
        return;
    }

    CategoryListModel *catModel = isIncome ? m_incomeCategories : m_expenseCategories;
    const int categoryId = idAt(catModel, categoryIndex);
    if (categoryId <= 0) {
        m_lastError = isIncome ? QStringLiteral("Выберите категорию дохода")
                               : QStringLiteral("Выберите категорию расхода");
        emit operationFinished(false, m_lastError);
        return;
    }

    const qint64 absCents = MoneyFormatter::instance().parseToCents(amountText);
    if (absCents <= 0) {
        m_lastError = QStringLiteral("Укажите сумму больше нуля");
        emit operationFinished(false, m_lastError);
        return;
    }
    const qint64 cents = isIncome ? absCents : -absCents;
    const int accountId = accountIdAt(accountIndex);
    const QString own = (ownership == QLatin1String("personal")) ? QStringLiteral("personal")
                                                                 : QStringLiteral("family");
    const QString date = QDateTime::currentDateTime().toString(Qt::ISODate);

    auto *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, isIncome]() {
        const int id = watcher->result();
        m_lastError = id > 0 ? QString() : QStringLiteral("Ошибка добавления");
        const QString okMsg = isIncome ? QStringLiteral("Доход добавлен") : QStringLiteral("Расход добавлен");
        emit operationFinished(id > 0, id > 0 ? okMsg : m_lastError);
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().addTransaction(
        accountId, categoryId, cents, date, description, own, false));
}

void TransactionViewModel::removeTransaction(int id)
{
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        const bool ok = watcher->result();
        m_lastError = ok ? QString() : QStringLiteral("Ошибка удаления");
        emit operationFinished(ok, ok ? QStringLiteral("Удалено") : m_lastError);
        watcher->deleteLater();
    });
    watcher->setFuture(DatabaseManager::instance().deleteTransaction(id));
}
