#pragma once

#include <QSqlDatabase>
#include <QString>
#include <functional>

class DaoBase
{
public:
    using DbOp = std::function<bool(QSqlDatabase &)>;

    static QString makeConnectionName();
    /// Opens a per-call SQLite connection, applies pragmas, runs op. Returns false on open/pragma/op failure.
    static bool withConnection(const QString &dbPath, const DbOp &op);
};
