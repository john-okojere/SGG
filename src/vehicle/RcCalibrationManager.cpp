#include "RcCalibrationManager.h"

#include "../access/PermissionManager.h"
#include "../auth/SessionManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "MavsdkVehicleManager.h"

#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/system.hpp>

#include <QDateTime>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <thread>

namespace {
constexpr int kChannelCount = 8;
constexpr int kSavedChannelCount = 6;
constexpr int kUnsetPwm = 0;

QString paramName(int channel, const char *suffix)
{
    return QStringLiteral("RC%1_%2").arg(channel).arg(QString::fromLatin1(suffix));
}

QString paramResultString(mavsdk::Param::Result result)
{
    switch (result) {
    case mavsdk::Param::Result::Success:
        return QStringLiteral("Success");
    case mavsdk::Param::Result::Timeout:
        return QStringLiteral("Timeout");
    case mavsdk::Param::Result::WrongType:
        return QStringLiteral("Wrong type");
    case mavsdk::Param::Result::ParamNameTooLong:
        return QStringLiteral("Parameter name too long");
    case mavsdk::Param::Result::DoesNotExist:
        return QStringLiteral("Parameter does not exist");
    case mavsdk::Param::Result::ValueOutOfRange:
        return QStringLiteral("Value out of range");
    case mavsdk::Param::Result::ConnectionError:
        return QStringLiteral("Connection error");
    case mavsdk::Param::Result::NoSystem:
        return QStringLiteral("No system");
    case mavsdk::Param::Result::Failed:
        return QStringLiteral("Operation failed");
    case mavsdk::Param::Result::PermissionDenied:
        return QStringLiteral("Permission denied");
    case mavsdk::Param::Result::ComponentNotFound:
        return QStringLiteral("Component not found");
    case mavsdk::Param::Result::ReadOnly:
        return QStringLiteral("Read only");
    default:
        return QStringLiteral("Parameter write failed");
    }
}
}

RcCalibrationManager::RcCalibrationManager(MavsdkVehicleManager *vehicle,
                                           SessionManager *session,
                                           PermissionManager *permissions,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_session(session),
      m_permissions(permissions),
      m_events(events)
{
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            if (!m_vehicle->connected() && m_monitoring) {
                stopMonitoring();
                setStatus(QStringLiteral("RC monitor stopped: flight controller disconnected."));
            }
        });
    }
}

RcCalibrationManager::~RcCalibrationManager()
{
    unsubscribe();
}

bool RcCalibrationManager::monitoring() const { return m_monitoring; }
bool RcCalibrationManager::rcAvailable() const { return m_rcAvailable; }
QString RcCalibrationManager::status() const { return m_status; }
QVariantList RcCalibrationManager::channels() const { return buildChannels(); }
int RcCalibrationManager::sampleCount() const { return m_sampleCount; }

void RcCalibrationManager::startMonitoring()
{
    if (!m_permissions || !m_permissions->hasPermission(QStringLiteral("can_configure_rc"))) {
        setStatus(QStringLiteral("RC calibration blocked: role does not allow RC configuration."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) {
        setStatus(QStringLiteral("Connect a flight controller before RC calibration."));
        return;
    }
    if (m_monitoring) {
        setStatus(QStringLiteral("RC monitor already running. Move sticks through their full travel."));
        return;
    }

    m_passthrough = std::make_shared<mavsdk::MavlinkPassthrough>(m_vehicle->system());
    auto handle = m_passthrough->subscribe_message(MAVLINK_MSG_ID_RC_CHANNELS, [this](const mavlink_message_t &message) {
        mavlink_rc_channels_t rc{};
        mavlink_msg_rc_channels_decode(&message, &rc);
        const std::array<uint16_t, kChannelCount> values = {
            rc.chan1_raw, rc.chan2_raw, rc.chan3_raw, rc.chan4_raw,
            rc.chan5_raw, rc.chan6_raw, rc.chan7_raw, rc.chan8_raw,
        };
        QMetaObject::invokeMethod(this, [this, values, count = int(rc.chancount), rssi = int(rc.rssi)]() {
            handleRcChannels(values.data(), count, rssi);
        }, Qt::QueuedConnection);
    });
    Q_UNUSED(handle)

    m_monitoring = true;
    setStatus(QStringLiteral("RC monitor running. Move every stick and switch through full travel, then center sticks and capture trim."));
    emit calibrationChanged();
}

void RcCalibrationManager::stopMonitoring()
{
    unsubscribe();
    m_monitoring = false;
    setStatus(QStringLiteral("RC monitor stopped."));
    emit calibrationChanged();
}

void RcCalibrationManager::resetCalibration()
{
    for (ChannelState &channel : m_channels) {
        channel = ChannelState{};
    }
    m_sampleCount = 0;
    m_rssi = -1;
    m_rcAvailable = false;
    setStatus(QStringLiteral("RC calibration samples reset."));
    emit calibrationChanged();
}

void RcCalibrationManager::captureTrim()
{
    bool captured = false;
    for (ChannelState &channel : m_channels) {
        if (channel.seen && channel.value > 0) {
            channel.trim = channel.value;
            captured = true;
        }
    }
    setStatus(captured
                  ? QStringLiteral("Trim captured from current stick positions.")
                  : QStringLiteral("No RC channel values available to capture trim."));
    emit calibrationChanged();
}

void RcCalibrationManager::saveCalibration()
{
    if (!canWriteCalibration()) {
        return;
    }

    QVariantList writes;
    for (int i = 0; i < kSavedChannelCount; ++i) {
        const ChannelState &channel = m_channels[i];
        if (!channel.seen || channel.minimum <= 0 || channel.maximum <= 0 || channel.maximum - channel.minimum < 200) {
            setStatus(QStringLiteral("Move channel %1 through full travel before saving.").arg(i + 1));
            return;
        }
        const int trim = channel.trim > 0 ? channel.trim : ((channel.minimum + channel.maximum) / 2);
        writes << QVariantMap{{QStringLiteral("name"), paramName(i + 1, "MIN")}, {QStringLiteral("value"), channel.minimum}};
        writes << QVariantMap{{QStringLiteral("name"), paramName(i + 1, "MAX")}, {QStringLiteral("value"), channel.maximum}};
        writes << QVariantMap{{QStringLiteral("name"), paramName(i + 1, "TRIM")}, {QStringLiteral("value"), trim}};
    }

    const auto system = m_vehicle->system();
    setStatus(QStringLiteral("Writing RC calibration to ArduPilot parameters..."));
    emit calibrationChanged();

    QPointer<RcCalibrationManager> self(this);
    std::thread([self, system, writes]() {
        mavsdk::Param param(system);
        QString failure;
        for (const QVariant &item : writes) {
            const QVariantMap write = item.toMap();
            const QString name = write.value(QStringLiteral("name")).toString();
            const int value = write.value(QStringLiteral("value")).toInt();
            const auto result = param.set_param_int(name.toStdString(), value);
            if (result != mavsdk::Param::Result::Success) {
                failure = QStringLiteral("%1: %2").arg(name, paramResultString(result));
                break;
            }
        }

        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, failure, writes]() {
            if (!self) {
                return;
            }
            if (!failure.isEmpty()) {
                self->setStatus(QStringLiteral("RC calibration write failed: %1").arg(failure));
                return;
            }
            self->setStatus(QStringLiteral("RC calibration saved to flight controller."));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("rc_calibration_saved"),
                                            QStringLiteral("info"),
                                            QStringLiteral("RC calibration parameters saved"),
                                            QJsonObject{
                                                {QStringLiteral("parameter_count"), writes.size()},
                                                {QStringLiteral("saved_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
                                            });
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void RcCalibrationManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit calibrationChanged();
}

void RcCalibrationManager::handleRcChannels(const uint16_t *values, int count, int rssi)
{
    const int boundedCount = std::clamp(count, 0, kChannelCount);
    bool changed = false;
    for (int i = 0; i < boundedCount; ++i) {
        const int value = values[i];
        if (value == 65535 || value <= 0) {
            continue;
        }
        ChannelState &channel = m_channels[i];
        channel.value = value;
        channel.minimum = channel.seen ? std::min(channel.minimum, value) : value;
        channel.maximum = channel.seen ? std::max(channel.maximum, value) : value;
        if (channel.trim == kUnsetPwm) {
            channel.trim = value;
        }
        channel.seen = true;
        changed = true;
    }
    if (!changed) {
        return;
    }
    ++m_sampleCount;
    m_rssi = rssi;
    m_rcAvailable = true;
    if (m_sampleCount == 1) {
        setStatus(QStringLiteral("RC input detected. Move sticks and switches through full travel."));
    } else {
        emit calibrationChanged();
    }
}

void RcCalibrationManager::unsubscribe()
{
    m_passthrough.reset();
}

bool RcCalibrationManager::canWriteCalibration() const
{
    if (!m_permissions || !m_permissions->hasPermission(QStringLiteral("can_configure_rc"))) {
        const_cast<RcCalibrationManager *>(this)->setStatus(QStringLiteral("RC calibration blocked: role does not allow RC configuration."));
        return false;
    }
    if (!m_permissions->hasPermission(QStringLiteral("can_write_vehicle_parameters"))) {
        const_cast<RcCalibrationManager *>(this)->setStatus(QStringLiteral("RC calibration blocked: role cannot write vehicle parameters."));
        return false;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        const_cast<RcCalibrationManager *>(this)->setStatus(QStringLiteral("RC calibration blocked: Control Center session/device approval required."));
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) {
        const_cast<RcCalibrationManager *>(this)->setStatus(QStringLiteral("Connect a flight controller before saving RC calibration."));
        return false;
    }
    return true;
}

QVariantList RcCalibrationManager::buildChannels() const
{
    QVariantList rows;
    for (int i = 0; i < kChannelCount; ++i) {
        const ChannelState &channel = m_channels[i];
        rows << QVariantMap{
            {QStringLiteral("index"), i + 1},
            {QStringLiteral("name"), QStringLiteral("CH%1").arg(i + 1)},
            {QStringLiteral("value"), channel.value},
            {QStringLiteral("minimum"), channel.minimum},
            {QStringLiteral("maximum"), channel.maximum},
            {QStringLiteral("trim"), channel.trim},
            {QStringLiteral("seen"), channel.seen},
            {QStringLiteral("range"), channel.seen ? channel.maximum - channel.minimum : 0},
            {QStringLiteral("rssi"), m_rssi},
        };
    }
    return rows;
}
