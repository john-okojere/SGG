#pragma once

#include <QObject>
#include <QString>

class ThemeController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bg READ bg CONSTANT)
    Q_PROPERTY(QString surface READ surface CONSTANT)
    Q_PROPERTY(QString surfaceAlt READ surfaceAlt CONSTANT)
    Q_PROPERTY(QString ink READ ink CONSTANT)
    Q_PROPERTY(QString muted READ muted CONSTANT)
    Q_PROPERTY(QString line READ line CONSTANT)
    Q_PROPERTY(QString purple READ purple CONSTANT)
    Q_PROPERTY(QString purple2 READ purple2 CONSTANT)
    Q_PROPERTY(QString purpleSoft READ purpleSoft CONSTANT)
    Q_PROPERTY(QString glass READ glass CONSTANT)
    Q_PROPERTY(QString glassSoft READ glassSoft CONSTANT)
    Q_PROPERTY(QString green READ green CONSTANT)
    Q_PROPERTY(QString amber READ amber CONSTANT)
    Q_PROPERTY(QString red READ red CONSTANT)
    Q_PROPERTY(QString white READ white CONSTANT)
    Q_PROPERTY(int topBarHeight READ topBarHeight CONSTANT)
    Q_PROPERTY(int toolRailWidth READ toolRailWidth CONSTANT)
    Q_PROPERTY(int panelRadius READ panelRadius CONSTANT)
    Q_PROPERTY(int controlRadius READ controlRadius CONSTANT)
    Q_PROPERTY(int spacing READ spacing CONSTANT)
    Q_PROPERTY(int animFast READ animFast CONSTANT)
    Q_PROPERTY(int anim READ anim CONSTANT)
    Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)

public:
    explicit ThemeController(QObject *parent = nullptr) : QObject(parent) {}

    QString bg() const { return QStringLiteral("#07050c"); }
    QString surface() const { return QStringLiteral("#f4f1f8"); }
    QString surfaceAlt() const { return QStringLiteral("#ebe7f2"); }
    QString ink() const { return QStringLiteral("#0e0b14"); }
    QString muted() const { return QStringLiteral("#766f82"); }
    QString line() const { return QStringLiteral("#d7cde5"); }
    QString purple() const { return QStringLiteral("#2c0057"); }
    QString purple2() const { return QStringLiteral("#4b128b"); }
    QString purpleSoft() const { return QStringLiteral("#7c45b8"); }
    QString glass() const { return QStringLiteral("#160720d9"); }
    QString glassSoft() const { return QStringLiteral("#210b32c8"); }
    QString green() const { return QStringLiteral("#22c55e"); }
    QString amber() const { return QStringLiteral("#f7c948"); }
    QString red() const { return QStringLiteral("#ef4444"); }
    QString white() const { return QStringLiteral("#ffffff"); }
    int topBarHeight() const { return 78; }
    int toolRailWidth() const { return 78; }
    int panelRadius() const { return 8; }
    int controlRadius() const { return 6; }
    int spacing() const { return 10; }
    int animFast() const { return 140; }
    int anim() const { return 220; }
    QString fontFamily() const { return QStringLiteral("Inter, Arial, sans-serif"); }
};
