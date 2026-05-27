#include "MapProviderController.h"

#include <QProcessEnvironment>

MapProviderController::MapProviderController(QObject *parent)
    : QObject(parent)
{
    m_apiKey = QProcessEnvironment::systemEnvironment()
                   .value(QStringLiteral("SKYGRID_ARCGIS_API_KEY"))
                   .trimmed();
    m_apiKeyConfigured = !m_apiKey.isEmpty();
    m_sdkPath = QProcessEnvironment::systemEnvironment()
                    .value(QStringLiteral("ARCGIS_RUNTIME_QT_PATH"))
                    .trimmed();
#ifdef SKYGRID_HAS_ARCGIS
    m_arcGisAvailable = true;
#else
    m_arcGisAvailable = false;
#endif
#ifdef SKYGRID_HAS_ARCGIS_QML
    m_arcGisQmlAvailable = true;
#else
    m_arcGisQmlAvailable = false;
#endif
    refreshProvider();
}

QString MapProviderController::providerName() const
{
    return QStringLiteral("SkyGrid Tile Map");
}

QString MapProviderController::providerStatus() const
{
    if (m_layerMode == "SAT") {
        return QStringLiteral("World Imagery tiles · Online");
    }
    if (m_layerMode == "3D") {
        return QStringLiteral("Terrain tile preview · Online");
    }
    return QStringLiteral("Topographic tiles · Online");
/*
    if (m_online && m_arcGisQmlAvailable) {
        if (m_layerMode == "SAT") {
            return QStringLiteral("ArcGIS World Imagery · Online");
        }
        if (m_layerMode == "3D") {
            return supports3D()
                ? QStringLiteral("ArcGIS 3D Scene · Online")
                : QStringLiteral("ArcGIS 3D requires SDK scene support");
        }
        return QStringLiteral("ArcGIS Topographic · Online");
    }
    if (m_online) {
        return QStringLiteral("ArcGIS SDK detected · QML surface unavailable");
    }
    if (!m_arcGisAvailable) {
        return m_sdkPath.isEmpty()
            ? QStringLiteral("Offline simulation · ARCGIS_RUNTIME_QT_PATH not set")
            : QStringLiteral("Offline simulation · ArcGIS SDK not detected");
    }
    if (!m_apiKeyConfigured) {
        return QStringLiteral("Offline simulation - missing SKYGRID_ARCGIS_API_KEY");
    }
    return QStringLiteral("Offline simulation");
*/
}

bool MapProviderController::online() const { return m_online; }
bool MapProviderController::apiKeyConfigured() const { return m_apiKeyConfigured; }
QString MapProviderController::layerMode() const { return m_layerMode; }
bool MapProviderController::loading() const { return m_loading; }
QString MapProviderController::errorMessage() const { return m_errorMessage; }
bool MapProviderController::arcGisAvailable() const { return m_arcGisAvailable; }
QString MapProviderController::sdkPath() const { return m_sdkPath; }
bool MapProviderController::arcGisQmlAvailable() const { return m_arcGisQmlAvailable; }
bool MapProviderController::supports3D() const { return m_online && m_arcGisQmlAvailable; }
QString MapProviderController::apiKey() const { return m_apiKey; }
QString MapProviderController::runtimeMode() const
{
    return QStringLiteral("tiles");
}

void MapProviderController::setLayerMode(const QString &mode)
{
    if (m_layerMode == mode) {
        return;
    }
    m_layerMode = mode;
    emit layerModeChanged();
    emit providerChanged();
}

void MapProviderController::refreshProvider()
{
    m_loading = true;
    m_online = true;
    m_errorMessage.clear();
    m_loading = false;
    emit providerChanged();
}

void MapProviderController::setProviderOnline(bool online)
{
    online = true;
    if (m_online == online) {
        return;
    }
    m_online = online;
    emit providerChanged();
}
