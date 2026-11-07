#include "DaoBase.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

QString DaoBase::makeConnectionName()
{
    return QStringLiteral("fb_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

bool DaoBase::withConnection(const QString &dbPath, const DbOp &op)
{
    const QString name = makeConnectionName();
    bool ok = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            qWarning("Failed to open DB %s: %s", qPrintable(name), qPrintable(db.lastError().text()));
            QSqlDatabase::removeDatabase(name);
            return false;
        }

        QSqlQuery pragma(db);
        if (!pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"))
            || !pragma.exec(QStringLiteral("PRAGMA busy_timeout = 5000"))) {
            qWarning("Failed to apply pragmas: %s", qPrintable(pragma.lastError().text()));
            db.close();
            QSqlDatabase::removeDatabase(name);
            return false;
        }
        // Best-effort; may fail on some temp/network filesystems.
        pragma.exec(QStringLiteral("PRAGMA journal_mode = WAL"));

        ok = op(db);
        db.close();
    }
    QSqlDatabase::removeDatabase(name);
    return ok;
}
