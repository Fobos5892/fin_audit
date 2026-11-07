#include "DatabaseManager.h"
#include "DaoBase.h"

#include <QDate>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QThreadPool>
#include <QtConcurrent>
#include <cmath>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

bool DatabaseManager::withDb(const std::function<bool(QSqlDatabase &)> &op) const
{
    return DaoBase::withConnection(m_dbPath, op);
}

template <typename T>
QFuture<T> DatabaseManager::runAsync(const std::function<T()> &fn)
{
    return QtConcurrent::run(fn);
}

void DatabaseManager::runOnReady(const std::function<void()> &fn)
{
    if (m_ready) {
        fn();
        return;
    }
    m_pendingReady.append(fn);
}

void DatabaseManager::initialize(const QString &dbPath)
{
    // Ensure qrc from static FamilyBudgetCore is registered into the process.
    Q_INIT_RESOURCE(resources);
    m_dbPath = dbPath;
    QThreadPool::globalInstance()->start([this]() {
        QString error;
        const bool ok = withDb([this, &error](QSqlDatabase &db) {
            if (!executeSchema(db)) {
                error = QStringLiteral("РћС€РёР±РєР° СЃС…РµРјС‹ Р‘Р”");
                return false;
            }
            if (!seedIfEmpty(db)) {
                error = QStringLiteral("РћС€РёР±РєР° seed Р‘Р”");
                return false;
            }
            return true;
        });

        QMetaObject::invokeMethod(this, [this, ok, error]() {
            if (!ok) {
                emit errorOccurred(error.isEmpty() ? QStringLiteral("РќРµ СѓРґР°Р»РѕСЃСЊ РѕС‚РєСЂС‹С‚СЊ Р±Р°Р·Сѓ РґР°РЅРЅС‹С…") : error);
                return;
            }
            m_ready = true;
            for (const auto &fn : std::as_const(m_pendingReady))
                fn();
            m_pendingReady.clear();
            emit databaseReady();
        }, Qt::QueuedConnection);
    });
}

bool DatabaseManager::executeSchema(QSqlDatabase &db)
{
    QFile file(QStringLiteral(":/sql/migrations/001_init.sql"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Cannot open migration resource");
        return false;
    }
    const QString script = QString::fromUtf8(file.readAll());
    const QStringList statements = script.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    QSqlQuery query(db);
    for (QString stmt : statements) {
        stmt = stmt.trimmed();
        if (stmt.isEmpty() || stmt.startsWith(QLatin1String("--")))
            continue;
        if (!query.exec(stmt)) {
            qWarning("Schema error: %s | %s", qPrintable(query.lastError().text()),
                     qPrintable(stmt.left(120)));
            return false;
        }
    }

    // Triggers contain ';' — must not go through the simple statement splitter above.
    const QStringList triggers = {
        QStringLiteral(
            "CREATE TRIGGER IF NOT EXISTS trg_tx_ai AFTER INSERT ON transactions "
            "BEGIN "
            "UPDATE accounts SET balance_cents = balance_cents + NEW.amount_cents "
            "WHERE id = NEW.account_id; "
            "END"),
        QStringLiteral(
            "CREATE TRIGGER IF NOT EXISTS trg_tx_ad AFTER DELETE ON transactions "
            "BEGIN "
            "UPDATE accounts SET balance_cents = balance_cents - OLD.amount_cents "
            "WHERE id = OLD.account_id; "
            "END"),
    };
    for (const QString &trg : triggers) {
        if (!query.exec(trg)) {
            qWarning("Trigger error: %s", qPrintable(query.lastError().text()));
            return false;
        }
    }

    QSqlQuery ver(db);
    if (!ver.exec(QStringLiteral("SELECT COUNT(*) FROM schema_version")))
        return false;
    if (ver.next() && ver.value(0).toInt() == 0) {
        if (!ver.exec(QStringLiteral("INSERT INTO schema_version(version) VALUES (1)")))
            return false;
    }
    return true;
}

bool DatabaseManager::seedIfEmpty(QSqlDatabase &db)
{
    QSqlQuery q(db);
    if (!q.exec(QStringLiteral("SELECT COUNT(*) FROM accounts")) || !q.next())
        return false;
    if (q.value(0).toInt() > 0)
        return true;

    // Card starts at 0: demo txs update balance via triggers.
    q.prepare(QStringLiteral(
        "INSERT INTO accounts(name, type, balance_cents, currency) VALUES (?,?,?,?), (?,?,?,?), (?,?,?,?)"));
    q.addBindValue(QString::fromUtf8(u8"Основная карта"));
    q.addBindValue(QStringLiteral("card"));
    q.addBindValue(0);
    q.addBindValue(QStringLiteral("RUB"));
    q.addBindValue(QString::fromUtf8(u8"Наличные"));
    q.addBindValue(QStringLiteral("cash"));
    q.addBindValue(2500000);
    q.addBindValue(QStringLiteral("RUB"));
    q.addBindValue(QString::fromUtf8(u8"Накопительный"));
    q.addBindValue(QStringLiteral("savings"));
    q.addBindValue(50000000);
    q.addBindValue(QStringLiteral("RUB"));
    if (!q.exec())
        return false;

    struct Cat {
        const char8_t *name;
        const char *type;
        const char *color;
    };
    const Cat cats[] = {
        {u8"Зарплата", "income", "#7C6BC4"},      // основной доход (работа)
        {u8"Подработка", "income", "#9B8FD9"},
        {u8"Дивиденды", "income", "#5B8FD9"},     // пассивный
        {u8"Продукты", "expense", "#D06B6B"},
        {u8"Транспорт", "expense", "#C49A5A"},
        {u8"Жильё", "expense", "#8B7BB8"},
        {u8"Кафе", "expense", "#C47A9A"},
        {u8"Здоровье", "expense", "#5FA8B8"},
        {u8"Развлечения", "expense", "#6B90B5"},
    };
    for (const auto &c : cats) {
        q.prepare(QStringLiteral(
            "INSERT INTO categories(name, type, icon, chart_color) VALUES (?, ?, 'default', ?)"));
        q.addBindValue(QString::fromUtf8(reinterpret_cast<const char *>(c.name)));
        q.addBindValue(QString::fromUtf8(c.type));
        q.addBindValue(QString::fromUtf8(c.color));
        if (!q.exec())
            return false;
    }

    const QString today = QDate::currentDate().toString(Qt::ISODate);
    const struct {
        int categoryId;
        qint64 amount;
        const char8_t *description;
        const char *ownership;
        const char *time;
    } txs[] = {
        {1, 12000000, u8"Зарплата", "family", "T10:00:00"},
        {3, 1500000, u8"Дивиденды SBER", "family", "T11:00:00"}, // пассив +
        {4, -450000, u8"Пятёрочка", "family", "T12:30:00"},
        {5, -180000, u8"Метро", "personal", "T08:15:00"},
        {7, -220000, u8"Ужин", "family", "T19:40:00"},
        {6, -3500000, u8"Ипотека/аренда", "family", "T09:00:00"},
    };
    for (const auto &tx : txs) {
        q.prepare(QStringLiteral(
            "INSERT INTO transactions(account_id, category_id, amount_cents, date, description, ownership) "
            "VALUES (1, ?, ?, ?, ?, ?)"));
        q.addBindValue(tx.categoryId);
        q.addBindValue(tx.amount);
        q.addBindValue(today + QString::fromUtf8(tx.time));
        q.addBindValue(QString::fromUtf8(reinterpret_cast<const char *>(tx.description)));
        q.addBindValue(QString::fromUtf8(tx.ownership));
        if (!q.exec())
            return false;
    }

    q.prepare(QStringLiteral(
        "INSERT INTO goals(name, target_cents, current_cents, deadline, auto_allocate) VALUES "
        "(?, ?, ?, ?, 1),(?, ?, ?, ?, 0)"));
    q.addBindValue(QString::fromUtf8(u8"Подушка безопасности"));
    q.addBindValue(50000000);
    q.addBindValue(15000000);
    q.addBindValue(QDate::currentDate().addYears(1).toString(Qt::ISODate));
    q.addBindValue(QString::fromUtf8(u8"Новая машина"));
    q.addBindValue(150000000);
    q.addBindValue(25000000);
    q.addBindValue(QDate::currentDate().addYears(3).toString(Qt::ISODate));
    if (!q.exec())
        return false;

    q.prepare(QStringLiteral(
        "INSERT INTO investments(ticker, name, quantity, avg_price_cents, current_price_cents, type) "
        "VALUES (?,?,?,?,?,?),(?,?,?,?,?,?)"));
    q.addBindValue(QStringLiteral("SBER"));
    q.addBindValue(QString::fromUtf8(u8"Сбербанк"));
    q.addBindValue(50);
    q.addBindValue(28000);
    q.addBindValue(29500);
    q.addBindValue(QStringLiteral("stock"));
    q.addBindValue(QStringLiteral("FXGD"));
    q.addBindValue(QString::fromUtf8(u8"Золото"));
    q.addBindValue(10);
    q.addBindValue(950000);
    q.addBindValue(980000);
    q.addBindValue(QStringLiteral("metal"));
    return q.exec();
}

QHash<QString, QString> DatabaseManager::loadSettingsSync()
{
    QHash<QString, QString> map;
    withDb([&](QSqlDatabase &db) {
        QSqlQuery q(db);
        if (!q.exec(QStringLiteral("SELECT key, value FROM settings")))
            return false;
        while (q.next())
            map.insert(q.value(0).toString(), q.value(1).toString());
        return true;
    });
    return map;
}

void DatabaseManager::setSetting(const QString &key, const QString &value)
{
    QThreadPool::globalInstance()->start([this, key, value]() {
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "INSERT INTO settings(key, value) VALUES(?, ?) "
                "ON CONFLICT(key) DO UPDATE SET value = excluded.value"));
            q.addBindValue(key);
            q.addBindValue(value);
            return q.exec();
        });
    });
}

QFuture<QVariantList> DatabaseManager::fetchAccounts()
{
    return runAsync<QVariantList>([this]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT id, name, type, balance_cents, currency FROM accounts ORDER BY id")))
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("name"), q.value(1));
                row.insert(QStringLiteral("type"), q.value(2));
                row.insert(QStringLiteral("balanceCents"), q.value(3));
                row.insert(QStringLiteral("currency"), q.value(4));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::addAccount(const QString &name, const QString &type, qint64 balanceCents,
                                         const QString &currency)
{
    return runAsync<int>([this, name, type, balanceCents, currency]() {
        int id = -1;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "INSERT INTO accounts(name, type, balance_cents, currency) VALUES(?, ?, ?, ?)"));
            q.addBindValue(name);
            q.addBindValue(type);
            q.addBindValue(balanceCents);
            q.addBindValue(currency);
            if (!q.exec())
                return false;
            id = q.lastInsertId().toInt();
            return true;
        });
        if (id > 0)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return id;
    });
}

QFuture<QVariantList> DatabaseManager::fetchCategories()
{
    return runAsync<QVariantList>([this]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT id, name, type, parent_id, icon, chart_color FROM categories ORDER BY type, name")))
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("name"), q.value(1));
                row.insert(QStringLiteral("type"), q.value(2));
                row.insert(QStringLiteral("parentId"), q.value(3));
                row.insert(QStringLiteral("icon"), q.value(4));
                row.insert(QStringLiteral("chartColor"), q.value(5));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::addCategory(const QString &name, const QString &type, const QString &icon,
                                          const QString &color, int parentId)
{
    return runAsync<int>([this, name, type, icon, color, parentId]() {
        int id = -1;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "INSERT INTO categories(name, type, parent_id, icon, chart_color) VALUES(?, ?, ?, ?, ?)"));
            q.addBindValue(name);
            q.addBindValue(type);
            q.addBindValue(parentId > 0 ? QVariant(parentId) : QVariant());
            q.addBindValue(icon);
            q.addBindValue(color);
            if (!q.exec())
                return false;
            id = q.lastInsertId().toInt();
            return true;
        });
        if (id > 0)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return id;
    });
}

QFuture<bool> DatabaseManager::updateCategoryColor(int id, const QString &hex)
{
    return runAsync<bool>([this, id, hex]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral("UPDATE categories SET chart_color = ? WHERE id = ?"));
            q.addBindValue(hex);
            q.addBindValue(id);
            ok = q.exec();
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<bool> DatabaseManager::deleteCategory(int id)
{
    return runAsync<bool>([this, id]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral("DELETE FROM categories WHERE id = ?"));
            q.addBindValue(id);
            ok = q.exec();
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<QVariantList> DatabaseManager::fetchTransactions(int limit)
{
    return runAsync<QVariantList>([this, limit]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "SELECT t.id, t.account_id, t.category_id, t.amount_cents, t.date, t.description, "
                "t.ownership, t.is_investment, c.name, c.chart_color, a.name "
                "FROM transactions t "
                "LEFT JOIN categories c ON c.id = t.category_id "
                "LEFT JOIN accounts a ON a.id = t.account_id "
                "ORDER BY t.date DESC LIMIT ?"));
            q.addBindValue(limit);
            if (!q.exec())
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("accountId"), q.value(1));
                row.insert(QStringLiteral("categoryId"), q.value(2));
                row.insert(QStringLiteral("amountCents"), q.value(3));
                row.insert(QStringLiteral("date"), q.value(4));
                row.insert(QStringLiteral("description"), q.value(5));
                row.insert(QStringLiteral("ownership"), q.value(6));
                row.insert(QStringLiteral("isInvestment"), q.value(7).toInt() == 1);
                row.insert(QStringLiteral("categoryName"), q.value(8));
                row.insert(QStringLiteral("chartColor"), q.value(9));
                row.insert(QStringLiteral("accountName"), q.value(10));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::addTransaction(int accountId, int categoryId, qint64 amountCents,
                                             const QString &dateIso, const QString &description,
                                             const QString &ownership, bool isInvestment)
{
    return runAsync<int>([this, accountId, categoryId, amountCents, dateIso, description, ownership,
                          isInvestment]() {
        int id = -1;
        const bool saved = withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "INSERT INTO transactions(account_id, category_id, amount_cents, date, description, "
                "ownership, is_investment) VALUES(?, ?, ?, ?, ?, ?, ?)"));
            q.addBindValue(accountId);
            q.addBindValue(categoryId > 0 ? QVariant(categoryId) : QVariant());
            q.addBindValue(amountCents);
            q.addBindValue(dateIso);
            q.addBindValue(description);
            q.addBindValue(ownership);
            q.addBindValue(isInvestment ? 1 : 0);
            if (!q.exec()) {
                qWarning("insert transaction failed: %s", qPrintable(q.lastError().text()));
                return false;
            }
            id = q.lastInsertId().toInt();
            return id > 0;
        });
        if (!saved || id <= 0)
            return -1;
        QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return id;
    });
}

QFuture<bool> DatabaseManager::deleteTransaction(int id)
{
    return runAsync<bool>([this, id]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery del(db);
            del.prepare(QStringLiteral("DELETE FROM transactions WHERE id = ?"));
            del.addBindValue(id);
            ok = del.exec() && del.numRowsAffected() > 0;
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<QVariantList> DatabaseManager::fetchGoals()
{
    return runAsync<QVariantList>([this]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT id, name, target_cents, current_cents, deadline, auto_allocate FROM goals ORDER BY id")))
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("name"), q.value(1));
                row.insert(QStringLiteral("targetCents"), q.value(2));
                row.insert(QStringLiteral("currentCents"), q.value(3));
                row.insert(QStringLiteral("deadline"), q.value(4));
                row.insert(QStringLiteral("autoAllocate"), q.value(5).toInt() == 1);
                const qint64 target = q.value(2).toLongLong();
                const qint64 current = q.value(3).toLongLong();
                const double progress = target > 0
                    ? (100.0 * static_cast<double>(current) / static_cast<double>(target))
                    : 0.0;
                row.insert(QStringLiteral("progress"), qMin(100.0, progress));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::addGoal(const QString &name, qint64 targetCents, qint64 currentCents,
                                      const QString &deadline, bool autoAllocate)
{
    return runAsync<int>([this, name, targetCents, currentCents, deadline, autoAllocate]() {
        int id = -1;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "INSERT INTO goals(name, target_cents, current_cents, deadline, auto_allocate) "
                "VALUES(?, ?, ?, ?, ?)"));
            q.addBindValue(name);
            q.addBindValue(targetCents);
            q.addBindValue(currentCents);
            q.addBindValue(deadline.isEmpty() ? QVariant() : QVariant(deadline));
            q.addBindValue(autoAllocate ? 1 : 0);
            if (!q.exec())
                return false;
            id = q.lastInsertId().toInt();
            return true;
        });
        if (id > 0)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return id;
    });
}

QFuture<bool> DatabaseManager::updateGoalProgress(int id, qint64 currentCents)
{
    return runAsync<bool>([this, id, currentCents]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral("UPDATE goals SET current_cents = ? WHERE id = ?"));
            q.addBindValue(currentCents);
            q.addBindValue(id);
            ok = q.exec();
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<bool> DatabaseManager::contributeToGoal(int id, qint64 addCents)
{
    return runAsync<bool>([this, id, addCents]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "UPDATE goals SET current_cents = current_cents + ? WHERE id = ? AND ? > 0"));
            q.addBindValue(addCents);
            q.addBindValue(id);
            q.addBindValue(addCents);
            ok = q.exec() && q.numRowsAffected() > 0;
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<bool> DatabaseManager::allocateToGoals(qint64 amountCents)
{
    return runAsync<bool>([this, amountCents]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QList<QPair<int, qint64>> gaps;
            qint64 totalGap = 0;
            {
                QSqlQuery q(db);
                if (!q.exec(QStringLiteral(
                        "SELECT id, target_cents, current_cents FROM goals WHERE auto_allocate = 1 ORDER BY id")))
                    return false;
                while (q.next()) {
                    const int id = q.value(0).toInt();
                    const qint64 gap = qMax<qint64>(0, q.value(1).toLongLong() - q.value(2).toLongLong());
                    if (gap > 0) {
                        gaps.append({id, gap});
                        totalGap += gap;
                    }
                }
                q.finish();
            }
            if (gaps.isEmpty() || amountCents <= 0) {
                ok = true;
                return true;
            }
            if (!db.transaction())
                return false;
            qint64 remaining = amountCents;
            for (int i = 0; i < gaps.size(); ++i) {
                qint64 share = (i == gaps.size() - 1)
                    ? remaining
                    : (amountCents * gaps[i].second) / totalGap;
                share = qMin(share, gaps[i].second);
                remaining -= share;
                QSqlQuery upd(db);
                upd.prepare(QStringLiteral(
                    "UPDATE goals SET current_cents = current_cents + ? WHERE id = ?"));
                upd.addBindValue(share);
                upd.addBindValue(gaps[i].first);
                if (!upd.exec()) {
                    db.rollback();
                    return false;
                }
                upd.finish();
            }
            ok = db.commit();
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<QVariantList> DatabaseManager::fetchInvestments()
{
    return runAsync<QVariantList>([this]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT id, ticker, name, quantity, avg_price_cents, current_price_cents, type "
                    "FROM investments ORDER BY ticker")))
                return false;
            while (q.next()) {
                QVariantMap row;
                const double qty = q.value(3).toDouble();
                const qint64 avg = q.value(4).toLongLong();
                const qint64 cur = q.value(5).toLongLong();
                const qint64 value = static_cast<qint64>(std::llround(qty * static_cast<double>(cur)));
                const qint64 cost = static_cast<qint64>(std::llround(qty * static_cast<double>(avg)));
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("ticker"), q.value(1));
                row.insert(QStringLiteral("name"), q.value(2));
                row.insert(QStringLiteral("quantity"), qty);
                row.insert(QStringLiteral("avgPriceCents"), avg);
                row.insert(QStringLiteral("currentPriceCents"), cur);
                row.insert(QStringLiteral("type"), q.value(6));
                row.insert(QStringLiteral("valueCents"), value);
                row.insert(QStringLiteral("pnlCents"), value - cost);
                const double pnlPct = cost > 0
                    ? (100.0 * static_cast<double>(value - cost) / static_cast<double>(cost))
                    : 0.0;
                row.insert(QStringLiteral("pnlPercent"), pnlPct);
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::upsertInvestmentTrade(const QString &ticker, const QString &name,
                                                    const QString &type, const QString &side,
                                                    double quantity, qint64 priceCents,
                                                    const QString &dateIso)
{
    return runAsync<int>([this, ticker, name, type, side, quantity, priceCents, dateIso]() {
        int investmentId = -1;
        withDb([&](QSqlDatabase &db) {
            if (!db.transaction())
                return false;
            double qty = 0;
            qint64 avg = 0;
            {
                QSqlQuery find(db);
                find.prepare(QStringLiteral(
                    "SELECT id, quantity, avg_price_cents FROM investments WHERE ticker = ?"));
                find.addBindValue(ticker);
                if (!find.exec()) {
                    db.rollback();
                    return false;
                }
                if (find.next()) {
                    investmentId = find.value(0).toInt();
                    qty = find.value(1).toDouble();
                    avg = find.value(2).toLongLong();
                }
                find.finish();
            }
            if (investmentId < 0) {
                QSqlQuery ins(db);
                ins.prepare(QStringLiteral(
                    "INSERT INTO investments(ticker, name, quantity, avg_price_cents, current_price_cents, type) "
                    "VALUES(?, ?, 0, 0, ?, ?)"));
                ins.addBindValue(ticker);
                ins.addBindValue(name);
                ins.addBindValue(priceCents);
                ins.addBindValue(type);
                if (!ins.exec()) {
                    db.rollback();
                    investmentId = -1;
                    return false;
                }
                investmentId = ins.lastInsertId().toInt();
                ins.finish();
            }

            if (side == QLatin1String("buy")) {
                const double newQty = qty + quantity;
                avg = newQty > 0
                    ? static_cast<qint64>(std::llround(
                          (qty * static_cast<double>(avg) + quantity * static_cast<double>(priceCents))
                          / newQty))
                    : priceCents;
                qty = newQty;
            } else {
                qty = qMax(0.0, qty - quantity);
            }

            {
                QSqlQuery upd(db);
                upd.prepare(QStringLiteral(
                    "UPDATE investments SET quantity = ?, avg_price_cents = ?, current_price_cents = ?, "
                    "name = ?, type = ? WHERE id = ?"));
                upd.addBindValue(qty);
                upd.addBindValue(avg);
                upd.addBindValue(priceCents);
                upd.addBindValue(name);
                upd.addBindValue(type);
                upd.addBindValue(investmentId);
                if (!upd.exec()) {
                    db.rollback();
                    investmentId = -1;
                    return false;
                }
                upd.finish();
            }

            {
                QSqlQuery trade(db);
                trade.prepare(QStringLiteral(
                    "INSERT INTO investment_trades(investment_id, side, quantity, price_cents, date) "
                    "VALUES(?, ?, ?, ?, ?)"));
                trade.addBindValue(investmentId);
                trade.addBindValue(side);
                trade.addBindValue(quantity);
                trade.addBindValue(priceCents);
                trade.addBindValue(dateIso);
                if (!trade.exec()) {
                    db.rollback();
                    investmentId = -1;
                    return false;
                }
                trade.finish();
            }
            if (!db.commit()) {
                investmentId = -1;
                return false;
            }
            return true;
        });
        if (investmentId > 0)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return investmentId;
    });
}

QFuture<bool> DatabaseManager::updateInvestmentPrice(int id, qint64 currentPriceCents)
{
    return runAsync<bool>([this, id, currentPriceCents]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral("UPDATE investments SET current_price_cents = ? WHERE id = ?"));
            q.addBindValue(currentPriceCents);
            q.addBindValue(id);
            ok = q.exec();
            return ok;
        });
        if (ok)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return ok;
    });
}

QFuture<QVariantList> DatabaseManager::fetchMortgages()
{
    return runAsync<QVariantList>([this]() {
        QVariantList list;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT id, name, principal_cents, rate_bps, term_months, start_date, "
                    "extra_payment_cents, remaining_cents FROM mortgages ORDER BY id")))
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), q.value(0));
                row.insert(QStringLiteral("name"), q.value(1));
                row.insert(QStringLiteral("principalCents"), q.value(2));
                row.insert(QStringLiteral("rateBps"), q.value(3));
                row.insert(QStringLiteral("termMonths"), q.value(4));
                row.insert(QStringLiteral("startDate"), q.value(5));
                row.insert(QStringLiteral("extraPaymentCents"), q.value(6));
                row.insert(QStringLiteral("remainingCents"), q.value(7));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<int> DatabaseManager::saveMortgage(const QVariantMap &data)
{
    return runAsync<int>([this, data]() {
        int id = data.value(QStringLiteral("id"), -1).toInt();
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (id > 0) {
                q.prepare(QStringLiteral(
                    "UPDATE mortgages SET name=?, principal_cents=?, rate_bps=?, term_months=?, "
                    "start_date=?, extra_payment_cents=?, remaining_cents=? WHERE id=?"));
                q.addBindValue(data.value(QStringLiteral("name")));
                q.addBindValue(data.value(QStringLiteral("principalCents")));
                q.addBindValue(data.value(QStringLiteral("rateBps")));
                q.addBindValue(data.value(QStringLiteral("termMonths")));
                q.addBindValue(data.value(QStringLiteral("startDate")));
                q.addBindValue(data.value(QStringLiteral("extraPaymentCents")));
                q.addBindValue(data.value(QStringLiteral("remainingCents")));
                q.addBindValue(id);
                if (!q.exec()) {
                    id = -1;
                    return false;
                }
                return true;
            }
            q.prepare(QStringLiteral(
                "INSERT INTO mortgages(name, principal_cents, rate_bps, term_months, start_date, "
                "extra_payment_cents, remaining_cents) VALUES(?,?,?,?,?,?,?)"));
            q.addBindValue(data.value(QStringLiteral("name")));
            q.addBindValue(data.value(QStringLiteral("principalCents")));
            q.addBindValue(data.value(QStringLiteral("rateBps")));
            q.addBindValue(data.value(QStringLiteral("termMonths")));
            q.addBindValue(data.value(QStringLiteral("startDate")));
            q.addBindValue(data.value(QStringLiteral("extraPaymentCents")));
            q.addBindValue(data.value(QStringLiteral("remainingCents")));
            if (!q.exec()) {
                id = -1;
                return false;
            }
            id = q.lastInsertId().toInt();
            return true;
        });
        if (id > 0)
            QMetaObject::invokeMethod(this, [this]() { emit dataChanged(); }, Qt::QueuedConnection);
        return id;
    });
}

QFuture<bool> DatabaseManager::replaceMortgageSchedule(int mortgageId, const QVariantList &rows, bool withExtra)
{
    return runAsync<bool>([this, mortgageId, rows, withExtra]() {
        bool ok = false;
        withDb([&](QSqlDatabase &db) {
            if (!db.transaction())
                return false;
            {
                QSqlQuery del(db);
                del.prepare(QStringLiteral(
                    "DELETE FROM mortgage_schedule WHERE mortgage_id = ? AND with_extra = ?"));
                del.addBindValue(mortgageId);
                del.addBindValue(withExtra ? 1 : 0);
                if (!del.exec()) {
                    db.rollback();
                    return false;
                }
                del.finish();
            }
            {
                QSqlQuery ins(db);
                ins.prepare(QStringLiteral(
                    "INSERT INTO mortgage_schedule(mortgage_id, month_index, payment_cents, interest_cents, "
                    "principal_cents, remaining_cents, with_extra) VALUES(?,?,?,?,?,?,?)"));
                for (const QVariant &v : rows) {
                    const QVariantMap row = v.toMap();
                    ins.addBindValue(mortgageId);
                    ins.addBindValue(row.value(QStringLiteral("monthIndex")));
                    ins.addBindValue(row.value(QStringLiteral("paymentCents")));
                    ins.addBindValue(row.value(QStringLiteral("interestCents")));
                    ins.addBindValue(row.value(QStringLiteral("principalCents")));
                    ins.addBindValue(row.value(QStringLiteral("remainingCents")));
                    ins.addBindValue(withExtra ? 1 : 0);
                    if (!ins.exec()) {
                        db.rollback();
                        return false;
                    }
                }
                ins.finish();
            }
            ok = db.commit();
            return ok;
        });
        return ok;
    });
}

namespace {

bool isPassiveIncomeCategoryName(const QString &name)
{
    const QString n = name.toLower();
    return n.contains(QStringLiteral("дивиденд")) || n.contains(QStringLiteral("процент"))
           || n.contains(QStringLiteral("пассив")) || n.contains(QStringLiteral("инвест"))
           || n.contains(QStringLiteral("аренд")) || n.contains(QStringLiteral("купон"))
           || n.contains(QStringLiteral("вклад"));
}

} // namespace

QFuture<QVariantList> DatabaseManager::expenseSlicesForMonth(int year, int month)
{
    return runAsync<QVariantList>([this, year, month]() {
        QVariantList list;
        const QString start = QStringLiteral("%1-%2-01")
                                  .arg(year, 4, 10, QLatin1Char('0'))
                                  .arg(month, 2, 10, QLatin1Char('0'));
        const QString end = QDate(year, month, 1).addMonths(1).toString(Qt::ISODate);
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "SELECT c.id, c.name, c.chart_color, SUM(-t.amount_cents) AS total "
                "FROM transactions t "
                "JOIN categories c ON c.id = t.category_id "
                "WHERE t.amount_cents < 0 AND t.date >= ? AND t.date < ? "
                "GROUP BY c.id ORDER BY total DESC"));
            q.addBindValue(start);
            q.addBindValue(end);
            if (!q.exec())
                return false;
            while (q.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("categoryId"), q.value(0));
                row.insert(QStringLiteral("label"), q.value(1));
                row.insert(QStringLiteral("color"), q.value(2));
                row.insert(QStringLiteral("valueCents"), q.value(3));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<QVariantList> DatabaseManager::incomeSlicesForMonth(int year, int month)
{
    return runAsync<QVariantList>([this, year, month]() {
        QVariantList list;
        const QString start = QStringLiteral("%1-%2-01")
                                  .arg(year, 4, 10, QLatin1Char('0'))
                                  .arg(month, 2, 10, QLatin1Char('0'));
        const QString end = QDate(year, month, 1).addMonths(1).toString(Qt::ISODate);
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "SELECT c.id, c.name, c.chart_color, SUM(t.amount_cents) AS total "
                "FROM transactions t "
                "JOIN categories c ON c.id = t.category_id "
                "WHERE c.type = 'income' AND t.amount_cents > 0 AND t.date >= ? AND t.date < ? "
                "GROUP BY c.id HAVING total > 0 ORDER BY total DESC"));
            q.addBindValue(start);
            q.addBindValue(end);
            if (!q.exec())
                return false;
            while (q.next()) {
                const QString label = q.value(1).toString();
                QVariantMap row;
                row.insert(QStringLiteral("categoryId"), q.value(0));
                row.insert(QStringLiteral("label"), label);
                row.insert(QStringLiteral("color"), q.value(2));
                row.insert(QStringLiteral("valueCents"), q.value(3));
                row.insert(QStringLiteral("kind"),
                           isPassiveIncomeCategoryName(label) ? QStringLiteral("passive")
                                                              : QStringLiteral("work"));
                list.append(row);
            }
            return true;
        });
        return list;
    });
}

QFuture<QVariantMap> DatabaseManager::monthTotals(int year, int month)
{
    return runAsync<QVariantMap>([this, year, month]() {
        QVariantMap map;
        map.insert(QStringLiteral("incomeCents"), 0);
        map.insert(QStringLiteral("expenseCents"), 0);
        map.insert(QStringLiteral("workIncomeCents"), 0);
        map.insert(QStringLiteral("passiveIncomeCents"), 0);
        const QString start = QStringLiteral("%1-%2-01")
                                  .arg(year, 4, 10, QLatin1Char('0'))
                                  .arg(month, 2, 10, QLatin1Char('0'));
        const QString end = QDate(year, month, 1).addMonths(1).toString(Qt::ISODate);
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            q.prepare(QStringLiteral(
                "SELECT COALESCE(c.name, ''), t.amount_cents "
                "FROM transactions t "
                "LEFT JOIN categories c ON c.id = t.category_id "
                "WHERE t.date >= ? AND t.date < ?"));
            q.addBindValue(start);
            q.addBindValue(end);
            if (!q.exec())
                return false;

            qint64 income = 0;
            qint64 expense = 0;
            qint64 workIncome = 0;
            qint64 passiveIncome = 0;
            while (q.next()) {
                const QString name = q.value(0).toString();
                const qint64 amount = q.value(1).toLongLong();
                if (amount > 0) {
                    income += amount;
                    if (isPassiveIncomeCategoryName(name))
                        passiveIncome += amount;
                    else
                        workIncome += amount;
                } else if (amount < 0) {
                    // Убытки пассивных категорий крутим на кольце доходов, не в расходах
                    if (isPassiveIncomeCategoryName(name))
                        passiveIncome += amount;
                    else
                        expense += -amount;
                }
            }
            map.insert(QStringLiteral("incomeCents"), income);
            map.insert(QStringLiteral("expenseCents"), expense);
            map.insert(QStringLiteral("workIncomeCents"), workIncome);
            map.insert(QStringLiteral("passiveIncomeCents"), passiveIncome);
            return true;
        });
        return map;
    });
}

QFuture<qint64> DatabaseManager::totalAccountsBalance()
{
    return runAsync<qint64>([this]() {
        qint64 total = 0;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral("SELECT COALESCE(SUM(balance_cents), 0) FROM accounts")) || !q.next())
                return false;
            total = q.value(0).toLongLong();
            return true;
        });
        return total;
    });
}

QFuture<qint64> DatabaseManager::totalGoalsCurrent()
{
    return runAsync<qint64>([this]() {
        qint64 total = 0;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral("SELECT COALESCE(SUM(current_cents), 0) FROM goals")) || !q.next())
                return false;
            total = q.value(0).toLongLong();
            return true;
        });
        return total;
    });
}

QFuture<qint64> DatabaseManager::totalPortfolioValue()
{
    return runAsync<qint64>([this]() {
        qint64 total = 0;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral(
                    "SELECT COALESCE(SUM(quantity * current_price_cents), 0) FROM investments"))
                || !q.next())
                return false;
            total = static_cast<qint64>(std::llround(q.value(0).toDouble()));
            return true;
        });
        return total;
    });
}

QFuture<qint64> DatabaseManager::totalMortgageRemaining()
{
    return runAsync<qint64>([this]() {
        qint64 total = 0;
        withDb([&](QSqlDatabase &db) {
            QSqlQuery q(db);
            if (!q.exec(QStringLiteral("SELECT COALESCE(SUM(remaining_cents), 0) FROM mortgages")) || !q.next())
                return false;
            total = q.value(0).toLongLong();
            return true;
        });
        return total;
    });
}

template QFuture<QVariantList> DatabaseManager::runAsync<QVariantList>(const std::function<QVariantList()> &);
template QFuture<QVariantMap> DatabaseManager::runAsync<QVariantMap>(const std::function<QVariantMap()> &);
template QFuture<int> DatabaseManager::runAsync<int>(const std::function<int()> &);
template QFuture<bool> DatabaseManager::runAsync<bool>(const std::function<bool()> &);
template QFuture<qint64> DatabaseManager::runAsync<qint64>(const std::function<qint64()> &);
