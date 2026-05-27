#pragma once

#include <QObject>
#include <QString>

class MapProviderController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString providerName READ providerName NOTIFY providerChanged)
    Q_PROPERTY(QString providerStatus READ providerStatus NOTIFY providerChanged)
    Q_PROPERTY(bool online READ online NOTIFY providerChanged)
    Q_PROPERTY(bool apiKeyConfigured READ apiKeyConfigured CONSTANT)
    Q_PROPERTY(QString layerMode READ layerMode WRITE setLayerMode NOTIFY layerModeChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY providerChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY providerChanged)
    Q_PROPERTY(bool arcGisAvailable READ arcGisAvailable CONSTANT)
    Q_PROPERTY(QString sdkPath READ sdkPath CONSTANT)
    Q_PROPERTY(QString runtimeMode READ runtimeMode NOTIFY providerChanged)
    Q_PROPERTY(bool arcGisQmlAvailable READ arcGisQmlAvailable CONSTANT)
    Q_PROPERTY(bool supports3D READ supports3D NOTIFY providerChanged)
    Q_PROPERTY(QString apiKey READ apiKey CONSTANT)

public:
    explicit MapProviderController(QObject *parent = nullptr);

    QString providerName() const;
    QString providerStatus() const;
    bool online() const;
    bool apiKeyConfigured() const;
    QString layerMode() const;
    bool loading() const;
    QString errorMessage() const;
    bool arcGisAvailable() const;
    QString sdkPath() const;
    QString runtimeMode() const;
    bool arcGisQmlAvailable() const;
    bool supports3D() const;
    QString apiKey() const;

    void setLayerMode(const QString &mode);

    Q_INVOKABLE void refreshProvider();
    Q_INVOKABLE void setProviderOnline(bool online);

signals:
    void providerChanged();
    void layerModeChanged();

private:
    bool m_apiKeyConfigured = false;
    bool m_arcGisAvailable = false;
    bool m_arcGisQmlAvailable = false;
    bool m_online = false;
    bool m_loading = false;
    QString m_layerMode = "SAT";
    QString m_errorMessage;
    QString m_sdkPath;
    QString m_apiKey;
};
