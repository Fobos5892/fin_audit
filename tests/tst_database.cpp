#include "database/DatabaseManager.h"

#include <QDateTime>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class TstDatabase : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void seedCreatesAccountsAndCategories();
    void addTransactionUpdatesBalance();
    void contributeToGoalIncrements();

private:
    QTemporaryDir m_dir;
};

void TstDatabase::initTestCase()
{
    QVERIFY(m_dir.isValid());
    auto &db = DatabaseManager::instance();
    QSignalSpy ready(&db, &DatabaseManager::databaseReady);
    QSignalSpy errors(&db, &DatabaseManager::errorOccurred);
    db.initialize(m_dir.filePath(QStringLiteral("test.db")));
    QVERIFY2(ready.wait(5000), errors.isEmpty() ? "databaseReady timeout"
                                                : errors.first().first().toString().toUtf8().constData());
    QVERIFY(db.isReady());
}

void TstDatabase::seedCreatesAccountsAndCategories()
{
    auto accounts = DatabaseManager::instance().fetchAccounts().result();
    auto categories = DatabaseManager::instance().fetchCategories().result();
    QVERIFY(accounts.size() >= 3);
    QVERIFY(categories.size() >= 5);
}

void TstDatabase::addTransactionUpdatesBalance()
{
    auto accounts = DatabaseManager::instance().fetchAccounts().result();
    QVERIFY(!accounts.isEmpty());
    const auto first = accounts.first().toMap();
    const int accountId = first.value(QStringLiteral("id")).toInt();
    const qint64 before = first.value(QStringLiteral("balanceCents")).toLongLong();
    QVERIFY(accountId > 0);

    const auto categories = DatabaseManager::instance().fetchCategories().result();
    QVERIFY(!categories.isEmpty());
    const int categoryId = categories.first().toMap().value(QStringLiteral("id")).toInt();
    QVERIFY(categoryId > 0);

    const int txId = DatabaseManager::instance()
                         .addTransaction(accountId, categoryId, -10000,
                                         QDateTime::currentDateTime().toString(Qt::ISODate),
                                         QStringLiteral("test"), QStringLiteral("family"), false)
                         .result();
    QVERIFY2(txId > 0, "addTransaction failed");

    const auto txs = DatabaseManager::instance().fetchTransactions(50).result();
    bool found = false;
    for (const auto &v : txs) {
        if (v.toMap().value(QStringLiteral("id")).toInt() == txId)
            found = true;
    }
    QVERIFY2(found, "transaction row missing after insert");

    accounts = DatabaseManager::instance().fetchAccounts().result();
    qint64 after = -1;
    for (const auto &v : accounts) {
        if (v.toMap().value(QStringLiteral("id")).toInt() == accountId)
            after = v.toMap().value(QStringLiteral("balanceCents")).toLongLong();
    }
    QCOMPARE(after, before - 10000);
}

void TstDatabase::contributeToGoalIncrements()
{
    auto goals = DatabaseManager::instance().fetchGoals().result();
    QVERIFY(!goals.isEmpty());
    const int id = goals.first().toMap().value(QStringLiteral("id")).toInt();
    const qint64 before = goals.first().toMap().value(QStringLiteral("currentCents")).toLongLong();
    QVERIFY(DatabaseManager::instance().contributeToGoal(id, 5000).result());
    goals = DatabaseManager::instance().fetchGoals().result();
    qint64 after = before;
    for (const auto &v : goals) {
        if (v.toMap().value(QStringLiteral("id")).toInt() == id)
            after = v.toMap().value(QStringLiteral("currentCents")).toLongLong();
    }
    QCOMPARE(after, before + 5000);
}

QTEST_MAIN(TstDatabase)
#include "tst_database.moc"
