#pragma once

#include <QString>
#include <QVariantMap>

struct FirmwarePackageInfo
{
    QString filePath;
    QString fileName;
    QString packageType;
    QString version;
    QString target;
    QString checksum;
    QString fileSize;
};

struct FirmwareBoardInfo
{
    bool connected = false;
    QString stack;
    QString autopilot;
    QString systemId;
    QString connectionUrl;
};

class FirmwareFlashAdapter
{
public:
    virtual ~FirmwareFlashAdapter() = default;
    virtual QString name() const = 0;
    virtual bool flashingSupported() const = 0;
    virtual QString unsupportedReason() const = 0;
    virtual bool validatesPackage(const FirmwarePackageInfo &package,
                                  const FirmwareBoardInfo &board,
                                  QString *reason) const = 0;
};
