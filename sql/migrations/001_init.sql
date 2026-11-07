PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS accounts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK(type IN ('card', 'cash', 'savings')),
    balance_cents INTEGER NOT NULL DEFAULT 0,
    currency TEXT NOT NULL DEFAULT 'RUB'
);

CREATE TABLE IF NOT EXISTS categories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK(type IN ('income', 'expense')),
    parent_id INTEGER REFERENCES categories(id) ON DELETE SET NULL,
    icon TEXT NOT NULL DEFAULT 'default',
    chart_color TEXT NOT NULL DEFAULT '#3B82F6'
);

CREATE TABLE IF NOT EXISTS transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id INTEGER NOT NULL REFERENCES accounts(id) ON DELETE CASCADE,
    category_id INTEGER REFERENCES categories(id) ON DELETE SET NULL,
    amount_cents INTEGER NOT NULL,
    date TEXT NOT NULL,
    description TEXT,
    ownership TEXT NOT NULL DEFAULT 'family' CHECK(ownership IN ('family', 'personal')),
    is_investment INTEGER NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_transactions_date ON transactions(date);
CREATE INDEX IF NOT EXISTS idx_transactions_category ON transactions(category_id);
CREATE INDEX IF NOT EXISTS idx_transactions_account ON transactions(account_id);

CREATE TABLE IF NOT EXISTS investments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ticker TEXT NOT NULL,
    name TEXT NOT NULL,
    quantity REAL NOT NULL DEFAULT 0,
    avg_price_cents INTEGER NOT NULL DEFAULT 0,
    current_price_cents INTEGER NOT NULL DEFAULT 0,
    type TEXT NOT NULL CHECK(type IN ('stock', 'bond', 'crypto', 'fiat', 'metal'))
);

CREATE TABLE IF NOT EXISTS investment_trades (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    investment_id INTEGER NOT NULL REFERENCES investments(id) ON DELETE CASCADE,
    side TEXT NOT NULL CHECK(side IN ('buy', 'sell')),
    quantity REAL NOT NULL,
    price_cents INTEGER NOT NULL,
    date TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS goals (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    target_cents INTEGER NOT NULL,
    current_cents INTEGER NOT NULL DEFAULT 0,
    deadline TEXT,
    auto_allocate INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS mortgages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL DEFAULT 'Ипотека',
    principal_cents INTEGER NOT NULL,
    rate_bps INTEGER NOT NULL,
    term_months INTEGER NOT NULL,
    start_date TEXT NOT NULL,
    extra_payment_cents INTEGER NOT NULL DEFAULT 0,
    remaining_cents INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS mortgage_schedule (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    mortgage_id INTEGER NOT NULL REFERENCES mortgages(id) ON DELETE CASCADE,
    month_index INTEGER NOT NULL,
    payment_cents INTEGER NOT NULL,
    interest_cents INTEGER NOT NULL,
    principal_cents INTEGER NOT NULL,
    remaining_cents INTEGER NOT NULL,
    with_extra INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_investments_ticker ON investments(ticker);
