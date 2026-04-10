#pragma once

#include <QObject>
#include <QString>

class EffectSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath NOTIFY imagePathChanged)

public:
    explicit EffectSettings(QObject* parent = nullptr);

    float blurRadius() const;
    float opacity() const;
    bool enabled() const;
    QString imagePath() const;

public slots:
    void setBlurRadius(float value);
    void setOpacity(float value);
    void setEnabled(bool value);
    void setImagePath(const QString& path);
    void reset();

signals:
    void blurRadiusChanged();
    void opacityChanged();
    void enabledChanged();
    void imagePathChanged();
    void settingsChanged();

private:
    float m_blurRadius = 10.0f;
    float m_opacity = 1.0f;
    bool m_enabled = true;
    QString m_imagePath;
};