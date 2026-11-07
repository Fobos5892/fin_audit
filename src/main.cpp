#include "core/MoneyFormatter.h"
#include "core/SettingsStore.h"
#include "core/SplashController.h"
#include "core/ThemeManager.h"
#include "database/DatabaseManager.h"
#include "viewmodels/CapitalViewModel.h"
#include "viewmodels/DashboardViewModel.h"
#include "viewmodels/GoalViewModel.h"
#include "viewmodels/InvestmentViewModel.h"
#include "viewmodels/MortgageViewModel.h"
#include "viewmodels/SettingsViewModel.h"
#include "viewmodels/TransactionViewModel.h"

#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QScreen>
#include <QStandardPaths>
#include <QtQml>

namespace {

void centerWindow(QQuickWindow *window)
{
    if (!window)
        return;
    QScreen *screen = window->screen() ? window->screen() : QGuiApplication::primaryScreen();
    if (!screen)
        return;
    const QRect geo = screen->availableGeometry();
    window->setX(geo.x() + (geo.width() - window->width()) / 2);
    window->setY(geo.y() + (geo.height() - window->height()) / 2);
}

} // namespace

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("FamilyBudget"));
    app.setApplicationName(QStringLiteral("Семейный Бюджет"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setWindowIcon(QIcon(QStringLiteral(":/resources/icons/app_icon_256.png")));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    qmlRegisterSingletonInstance("FamilyBudget", 1, 0, "ThemeManager", &ThemeManager::instance());
    qmlRegisterSingletonInstance("FamilyBudget", 1, 0, "SettingsStore", &SettingsStore::instance());
    qmlRegisterSingletonInstance("FamilyBudget", 1, 0, "MoneyFormatter", &MoneyFormatter::instance());

    SplashController splashCtrl;
    qmlRegisterSingletonInstance("FamilyBudget", 1, 0, "SplashCtrl", &splashCtrl);

    // 1) Lightweight splash appears first
    QQmlApplicationEngine splashEngine;
    splashEngine.rootContext()->setContextProperty(QStringLiteral("SplashCtrl"), &splashCtrl);
    const QUrl splashUrl(QStringLiteral("qrc:/qml/splash/SplashWindow.qml"));
    splashEngine.load(splashUrl);
    if (splashEngine.rootObjects().isEmpty())
        return -1;

    auto *splashWindow = qobject_cast<QQuickWindow *>(splashEngine.rootObjects().constFirst());
    centerWindow(splashWindow);

    // 2) Heavy subsystems while splash animates
    DashboardViewModel dashboardVm;
    TransactionViewModel transactionVm;
    InvestmentViewModel investmentVm;
    GoalViewModel goalVm;
    MortgageViewModel mortgageVm;
    SettingsViewModel settingsVm;
    CapitalViewModel capitalVm;

    QObject::connect(&DatabaseManager::instance(), &DatabaseManager::errorOccurred, &app,
                     [&splashCtrl](const QString &message) {
                         qCritical("DB error: %s", qPrintable(message));
                         splashCtrl.setStatus(QStringLiteral("Ошибка БД"));
                     });
    QObject::connect(&DatabaseManager::instance(), &DatabaseManager::databaseReady, &splashCtrl,
                     [&splashCtrl]() {
                         splashCtrl.markDbReady();
                         SettingsStore::instance().loadFromDatabase();
                     });

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    splashCtrl.setStatus(QStringLiteral("Открытие базы данных…"));
    DatabaseManager::instance().initialize(dataDir + QStringLiteral("/family_budget.db"));

    QQmlApplicationEngine mainEngine;
    mainEngine.addImportPath(QStringLiteral("qrc:/qml"));
    auto *ctx = mainEngine.rootContext();
    ctx->setContextProperty(QStringLiteral("DashboardVM"), &dashboardVm);
    ctx->setContextProperty(QStringLiteral("TransactionVM"), &transactionVm);
    ctx->setContextProperty(QStringLiteral("InvestmentVM"), &investmentVm);
    ctx->setContextProperty(QStringLiteral("GoalVM"), &goalVm);
    ctx->setContextProperty(QStringLiteral("MortgageVM"), &mortgageVm);
    ctx->setContextProperty(QStringLiteral("SettingsVM"), &settingsVm);
    ctx->setContextProperty(QStringLiteral("CapitalVM"), &capitalVm);

    QQuickWindow *mainWindow = nullptr;
    bool handedOff = false;

    const auto handoff = [&]() {
        if (handedOff)
            return;
        handedOff = true;
        if (mainWindow) {
            centerWindow(mainWindow);
            mainWindow->setVisible(true);
            mainWindow->raise();
            mainWindow->requestActivate();
        }
        if (splashWindow) {
            splashWindow->close();
        }
    };

    const QUrl mainUrl(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &mainEngine, &QQmlApplicationEngine::objectCreated, &app,
        [&](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == mainUrl) {
                QCoreApplication::exit(-1);
                return;
            }
            if (objUrl != mainUrl)
                return;

            mainWindow = qobject_cast<QQuickWindow *>(obj);
            if (mainWindow) {
                mainWindow->setIcon(QIcon(QStringLiteral(":/resources/icons/app_icon_256.png")));
                mainWindow->setVisible(false);
                centerWindow(mainWindow);
            }
            splashCtrl.setStatus(QStringLiteral("Прогрев первой страницы…"));
            splashCtrl.markUiReady();
        },
        Qt::QueuedConnection);

    splashCtrl.setStatus(QStringLiteral("Сборка интерфейса…"));
    splashCtrl.bumpProgress(0.28);
    mainEngine.load(mainUrl);
    if (mainEngine.rootObjects().isEmpty())
        return -1;

    QObject::connect(&splashCtrl, &SplashController::splashFinished, &app, handoff, Qt::QueuedConnection);

    return app.exec();
}
