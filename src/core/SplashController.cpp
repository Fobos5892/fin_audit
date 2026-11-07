#include "SplashController.h"

#include <QtGlobal>

SplashController::SplashController(QObject *parent)
    : QObject(parent)
{
    m_timer.start();
}

void SplashController::markSplashVisible()
{
    setStatus(QStringLiteral("Инициализация ядра…"));
    bumpProgress(0.12);
}

void SplashController::markDbReady()
{
    if (m_dbReady)
        return;
    m_dbReady = true;
    setStatus(QStringLiteral("База данных готова"));
    bumpProgress(0.62);
    recompute();
}

void SplashController::markUiReady()
{
    if (m_uiReady)
        return;
    m_uiReady = true;
    setStatus(QStringLiteral("Интерфейс собран"));
    bumpProgress(0.92);
    recompute();
}

void SplashController::markMinAnimationDone()
{
    if (m_minTimeElapsed)
        return;
    m_minTimeElapsed = true;
    recompute();
}

void SplashController::notifySplashFinished()
{
    emit splashFinished();
}

void SplashController::setStatus(const QString &text)
{
    if (m_status == text)
        return;
    m_status = text;
    emit statusTextChanged();
}

void SplashController::bumpProgress(double target)
{
    setProgress(qMax(m_progress, qBound(0.0, target, 1.0)));
}

void SplashController::setProgress(double value)
{
    const double v = qBound(0.0, value, 1.0);
    if (qFuzzyCompare(m_progress, v))
        return;
    m_progress = v;
    emit progressChanged();
}

void SplashController::recompute()
{
    emit loadCompleteChanged();
    emit canDismissChanged();

    if (loadComplete()) {
        setStatus(QStringLiteral("Готово"));
        setProgress(1.0);
    }
}
