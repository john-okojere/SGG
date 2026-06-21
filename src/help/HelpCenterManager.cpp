#include "HelpCenterManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QStringConverter>
#include <QTextStream>

#include <algorithm>

HelpCenterManager::HelpCenterManager(QObject *parent)
    : QObject(parent)
{
    reload();
}

QString HelpCenterManager::documentText() const { return m_documentText; }
QString HelpCenterManager::documentPath() const { return m_documentPath; }
QString HelpCenterManager::status() const { return m_status; }

QVariantList HelpCenterManager::sections() const
{
    QVariantList rows;
    for (int i = 0; i < m_sections.size(); ++i) {
        const Section &section = m_sections.at(i);
        if (section.level <= 3) {
            rows << sectionToMap(section, i);
        }
    }
    return rows;
}

QVariantList HelpCenterManager::roleGuides() const
{
    return entriesForTitles({
        QStringLiteral("Command Center Administrator"),
        QStringLiteral("Organization Administrator"),
        QStringLiteral("Fleet Manager"),
        QStringLiteral("Pilot"),
        QStringLiteral("Manufacturer Administrator"),
        QStringLiteral("Manufacturer Engineer"),
        QStringLiteral("Auditor"),
        QStringLiteral("Technical Support"),
        QStringLiteral("Developer or Integrator")
    });
}

QVariantList HelpCenterManager::quickStarts() const
{
    return entriesForTitles({
        QStringLiteral("First Login and Trusted Device Workflow"),
        QStringLiteral("Connecting an Aircraft"),
        QStringLiteral("Mission Planning Workflow"),
        QStringLiteral("Preflight and Mission Start Workflow"),
        QStringLiteral("Pilot Mode Workflow"),
        QStringLiteral("Manufacturer Release Workflow"),
        QStringLiteral("Firmware Workflow"),
        QStringLiteral("Telemetry Sync Workflow")
    });
}

QVariantList HelpCenterManager::faq() const
{
    return entriesForTitles({
        QStringLiteral("Cannot Login"),
        QStringLiteral("Trusted Device Rejected or Pending"),
        QStringLiteral("No Permissions Visible"),
        QStringLiteral("Aircraft Not Connecting"),
        QStringLiteral("No Telemetry"),
        QStringLiteral("Mission Upload Failure"),
        QStringLiteral("Mission Start Failure"),
        QStringLiteral("Sync Failure"),
        QStringLiteral("Firmware Upload Failure"),
        QStringLiteral("GPS Issues"),
        QStringLiteral("Connection Loss")
    });
}

void HelpCenterManager::reload()
{
    m_documentText.clear();
    m_documentPath.clear();
    m_sections.clear();

    for (const QString &path : candidatePaths()) {
        QFile file(path);
        if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        m_documentText = stream.readAll();
        m_documentPath = path;
        m_status = QStringLiteral("Loaded help guide from %1").arg(path);
        parseSections();
        emit documentChanged();
        return;
    }

    m_status = QStringLiteral("SkyGrid help guide was not found.");
    emit documentChanged();
}

QVariantList HelpCenterManager::search(const QString &query) const
{
    const QStringList terms = query.simplified().toLower().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (terms.isEmpty()) {
        return {};
    }

    struct Hit {
        int score = 0;
        QVariantMap row;
    };

    QList<Hit> hits;
    for (int i = 0; i < m_sections.size(); ++i) {
        const Section &section = m_sections.at(i);
        const QString title = section.title.toLower();
        const QString body = section.text.toLower();
        int score = 0;
        for (const QString &term : terms) {
            if (title.contains(term)) {
                score += 5;
            }
            if (body.contains(term)) {
                score += 1;
            }
        }
        if (score > 0) {
            hits << Hit{score, sectionToMap(section, i, excerptFor(section.text, terms))};
        }
    }

    std::sort(hits.begin(), hits.end(), [](const Hit &left, const Hit &right) {
        return left.score > right.score;
    });

    QVariantList rows;
    const int maxRows = qMin(25, hits.size());
    for (int i = 0; i < maxRows; ++i) {
        QVariantMap row = hits.at(i).row;
        row.insert(QStringLiteral("score"), hits.at(i).score);
        rows << row;
    }
    return rows;
}

QString HelpCenterManager::sectionText(const QString &title) const
{
    const QString target = title.trimmed();
    if (target.isEmpty()) {
        return m_documentText;
    }
    for (const Section &section : m_sections) {
        if (section.title.compare(target, Qt::CaseInsensitive) == 0) {
            return section.text.trimmed();
        }
    }
    return m_documentText;
}

QString HelpCenterManager::contextHelp(const QString &contextKey) const
{
    const QString key = normalizedKey(contextKey);
    static const QHash<QString, QString> titles{
        {QStringLiteral("dashboard"), QStringLiteral("Dashboard")},
        {QStringLiteral("home"), QStringLiteral("Dashboard")},
        {QStringLiteral("mission_planner"), QStringLiteral("Mission Planner")},
        {QStringLiteral("planner"), QStringLiteral("Mission Planner")},
        {QStringLiteral("flight_data"), QStringLiteral("Flight Data")},
        {QStringLiteral("pilot"), QStringLiteral("Pilot Mode")},
        {QStringLiteral("pilot_mode"), QStringLiteral("Pilot Mode")},
        {QStringLiteral("manufacturer"), QStringLiteral("Manufacturer Workspace")},
        {QStringLiteral("manufacturer_workspace"), QStringLiteral("Manufacturer Workspace")},
        {QStringLiteral("command_center_sync"), QStringLiteral("Command Center Sync")},
        {QStringLiteral("gcs_tools"), QStringLiteral("GCS Tools Workspace")},
        {QStringLiteral("logs"), QStringLiteral("Logs / Analysis")},
        {QStringLiteral("simulation"), QStringLiteral("Simulation")}
    };
    return sectionText(titles.value(key, QStringLiteral("Help Center")));
}

QStringList HelpCenterManager::candidatePaths() const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString currentDir = QDir::currentPath();
    return {
        QDir(currentDir).filePath(QStringLiteral("docs/SkyGrid_GCS_User_Guide.md")),
        QDir(currentDir).filePath(QStringLiteral("../docs/SkyGrid_GCS_User_Guide.md")),
        QDir(appDir).filePath(QStringLiteral("docs/SkyGrid_GCS_User_Guide.md")),
        QDir(appDir).filePath(QStringLiteral("../docs/SkyGrid_GCS_User_Guide.md")),
        QDir(appDir).filePath(QStringLiteral("../../docs/SkyGrid_GCS_User_Guide.md")),
        QStringLiteral(":/qt/qml/SkyGrid/docs/SkyGrid_GCS_User_Guide.md"),
        QStringLiteral(":/docs/SkyGrid_GCS_User_Guide.md")
    };
}

void HelpCenterManager::parseSections()
{
    m_sections.clear();
    Section current;
    QString currentCategory;
    const QStringList lines = m_documentText.split(QLatin1Char('\n'));

    auto flush = [this, &current]() {
        if (!current.title.isEmpty()) {
            m_sections << current;
        }
    };

    for (const QString &line : lines) {
        int level = 0;
        while (level < line.size() && line.at(level) == QLatin1Char('#')) {
            ++level;
        }
        const bool heading = level > 0 && level <= 6 && line.size() > level && line.at(level).isSpace();
        if (heading) {
            flush();
            const QString title = line.mid(level).trimmed();
            if (level <= 2) {
                currentCategory = title;
            }
            current = Section{level, title, currentCategory, line + QLatin1Char('\n')};
            continue;
        }
        if (!current.title.isEmpty()) {
            current.text += line + QLatin1Char('\n');
        }
    }
    flush();
}

QVariantList HelpCenterManager::entriesForTitles(const QStringList &titles) const
{
    QVariantList rows;
    for (const QString &title : titles) {
        for (int i = 0; i < m_sections.size(); ++i) {
            const Section &section = m_sections.at(i);
            if (section.title.compare(title, Qt::CaseInsensitive) == 0) {
                rows << sectionToMap(section, i, excerptFor(section.text, {title.toLower()}));
                break;
            }
        }
    }
    return rows;
}

QVariantMap HelpCenterManager::sectionToMap(const Section &section, int index, const QString &excerpt) const
{
    return QVariantMap{
        {QStringLiteral("index"), index},
        {QStringLiteral("title"), section.title},
        {QStringLiteral("category"), section.category},
        {QStringLiteral("level"), section.level},
        {QStringLiteral("excerpt"), excerpt.isEmpty() ? section.text.simplified().left(220) : excerpt}
    };
}

QString HelpCenterManager::excerptFor(const QString &text, const QStringList &terms) const
{
    const QString simplified = text.simplified();
    const QString lower = simplified.toLower();
    int first = -1;
    for (const QString &term : terms) {
        const int index = lower.indexOf(term);
        if (index >= 0 && (first < 0 || index < first)) {
            first = index;
        }
    }
    if (first < 0) {
        return simplified.left(220);
    }
    const int start = qMax(0, first - 90);
    const int length = qMin(260, simplified.size() - start);
    QString excerpt = simplified.mid(start, length).trimmed();
    if (start > 0) {
        excerpt.prepend(QStringLiteral("... "));
    }
    if (start + length < simplified.size()) {
        excerpt.append(QStringLiteral(" ..."));
    }
    return excerpt;
}

QString HelpCenterManager::normalizedKey(QString value)
{
    value = value.trimmed().toLower();
    value.replace(QLatin1Char('-'), QLatin1Char('_'));
    value.replace(QLatin1Char(' '), QLatin1Char('_'));
    return value;
}
