#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;

class ProtocolTestManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList testRows READ testRows NOTIFY testsChanged)
    Q_PROPERTY(QVariantList artifactRows READ artifactRows NOTIFY testsChanged)
    Q_PROPERTY(QString artifactDirectory READ artifactDirectory NOTIFY testsChanged)
    Q_PROPERTY(QString status READ status NOTIFY testsChanged)
    Q_PROPERTY(bool running READ running NOTIFY testsChanged)

public:
    explicit ProtocolTestManager(AccessManager *access,
                                 GcsEventSyncManager *events,
                                 QObject *parent = nullptr);

    QVariantList testRows() const;
    QVariantList artifactRows() const;
    QString artifactDirectory() const;
    QString status() const;
    bool running() const;

    Q_INVOKABLE void runAll();
    Q_INVOKABLE void runBootloaderSelfTest();
    Q_INVOKABLE void createFirmwareSamples();
    Q_INVOKABLE void createSampleTlog();
    Q_INVOKABLE void runOptionalHardwareSelfTest();
    Q_INVOKABLE void prepareRtspTestProfile();
    Q_INVOKABLE void clear();

signals:
    void testsChanged();

private:
    QVariantMap row(const QString &name, const QString &value, const QString &state = QStringLiteral("ready")) const;
    bool authorize(const QString &action, const QString &label);
    QString ensureArtifactDirectory() const;
    QByteArray sampleFirmwareImage() const;
    QByteArray makeIntelHex(const QByteArray &image) const;
    QByteArray makeJsonFirmwarePackage(const QByteArray &image, const QString &stack) const;
    QByteArray makeMavlinkV2Frame(quint8 sequence, quint8 systemId, quint8 componentId, quint32 messageId, const QByteArray &payload) const;
    quint32 crc32(const QByteArray &bytes) const;
    void appendTest(const QString &name, const QString &value, const QString &state = QStringLiteral("ready"));
    void appendArtifact(const QString &name, const QString &path);
    void setStatus(const QString &status);
    void audit(const QString &eventType, const QString &severity, const QString &message) const;

    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_testRows;
    QVariantList m_artifactRows;
    QString m_status = "Protocol test mode idle.";
    bool m_running = false;
};
