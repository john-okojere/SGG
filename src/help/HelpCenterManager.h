#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class HelpCenterManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString documentText READ documentText NOTIFY documentChanged)
    Q_PROPERTY(QString documentPath READ documentPath NOTIFY documentChanged)
    Q_PROPERTY(QVariantList sections READ sections NOTIFY documentChanged)
    Q_PROPERTY(QVariantList roleGuides READ roleGuides NOTIFY documentChanged)
    Q_PROPERTY(QVariantList quickStarts READ quickStarts NOTIFY documentChanged)
    Q_PROPERTY(QVariantList faq READ faq NOTIFY documentChanged)
    Q_PROPERTY(QString status READ status NOTIFY documentChanged)

public:
    explicit HelpCenterManager(QObject *parent = nullptr);

    QString documentText() const;
    QString documentPath() const;
    QVariantList sections() const;
    QVariantList roleGuides() const;
    QVariantList quickStarts() const;
    QVariantList faq() const;
    QString status() const;

    Q_INVOKABLE void reload();
    Q_INVOKABLE QVariantList search(const QString &query) const;
    Q_INVOKABLE QString sectionText(const QString &title) const;
    Q_INVOKABLE QString contextHelp(const QString &contextKey) const;

signals:
    void documentChanged();

private:
    struct Section {
        int level = 0;
        QString title;
        QString category;
        QString text;
    };

    QStringList candidatePaths() const;
    void parseSections();
    QVariantList entriesForTitles(const QStringList &titles) const;
    QVariantMap sectionToMap(const Section &section, int index, const QString &excerpt = QString()) const;
    QString excerptFor(const QString &text, const QStringList &terms) const;
    static QString normalizedKey(QString value);

    QString m_documentText;
    QString m_documentPath;
    QString m_status = "Help guide not loaded.";
    QList<Section> m_sections;
};
