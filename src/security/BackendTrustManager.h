#pragma once

#include <QObject>
#include <QString>

class BackendTrustManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString baseUrl READ baseUrl NOTIFY backendChanged)
    Q_PROPERTY(QString websocketUrl READ websocketUrl NOTIFY backendChanged)
    Q_PROPERTY(bool productionSecure READ productionSecure NOTIFY backendChanged)

public:
    explicit BackendTrustManager(QObject *parent = nullptr);

    QString baseUrl() const;
    QString websocketUrl() const;
    bool productionSecure() const;
    QString makeUrl(const QString &path) const;
    QString makeWebSocketUrl(const QString &pathWithQuery) const;

    Q_INVOKABLE void setBaseUrl(const QString &baseUrl);

signals:
    void backendChanged();

private:
    QString normalizedBaseUrl(const QString &value) const;

    QString m_baseUrl;
};
