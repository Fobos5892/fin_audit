#pragma once

#include <QObject>
#include <QString>

class SettingsStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hideChromeForRecording READ hideChromeForRecording WRITE setHideChromeForRecording NOTIFY hideChromeChanged)

public:
    static SettingsStore &instance();

    bool hideChromeForRecording() const { return m_hideChrome; }
    void setHideChromeForRecording(bool hide);

    void loadFromDatabase();
    void persistKey(const QString &key, const QString &value);

signals:
    void hideChromeChanged();

private:
    explicit SettingsStore(QObject *parent = nullptr);

    bool m_hideChrome = false;
};
