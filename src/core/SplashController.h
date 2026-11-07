#pragma once

#include <QObject>
#include <QString>
#include <QElapsedTimer>

class SplashController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool dbReady READ dbReady NOTIFY loadCompleteChanged)
    Q_PROPERTY(bool uiReady READ uiReady NOTIFY loadCompleteChanged)
    Q_PROPERTY(bool loadComplete READ loadComplete NOTIFY loadCompleteChanged)
    Q_PROPERTY(bool canDismiss READ canDismiss NOTIFY canDismissChanged)
    Q_PROPERTY(int elapsedMs READ elapsedMs NOTIFY progressChanged)

public:
    explicit SplashController(QObject *parent = nullptr);

    double progress() const { return m_progress; }
    QString statusText() const { return m_status; }
    bool dbReady() const { return m_dbReady; }
    bool uiReady() const { return m_uiReady; }
    bool loadComplete() const { return m_dbReady && m_uiReady; }
    bool canDismiss() const { return loadComplete() && m_minTimeElapsed; }
    int elapsedMs() const { return static_cast<int>(m_timer.elapsed()); }

    Q_INVOKABLE void markSplashVisible();
    Q_INVOKABLE void markDbReady();
    Q_INVOKABLE void markUiReady();
    Q_INVOKABLE void markMinAnimationDone();
    Q_INVOKABLE void setStatus(const QString &text);
    Q_INVOKABLE void bumpProgress(double target);
    Q_INVOKABLE void notifySplashFinished();

signals:
    void progressChanged();
    void statusTextChanged();
    void loadCompleteChanged();
    void canDismissChanged();
    void splashFinished();

private:
    void recompute();
    void setProgress(double value);

    double m_progress = 0.05;
    QString m_status = QStringLiteral("Запуск…");
    bool m_dbReady = false;
    bool m_uiReady = false;
    bool m_minTimeElapsed = false;
    QElapsedTimer m_timer;
};
