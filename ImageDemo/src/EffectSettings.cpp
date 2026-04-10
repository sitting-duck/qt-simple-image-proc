#include "EffectSettings.h"
#include <QtGlobal>

EffectSettings::EffectSettings(QObject* parent)
    : QObject(parent)
{
}

float EffectSettings::blurRadius() const
{
    return m_blurRadius;
}

float EffectSettings::opacity() const
{
    return m_opacity;
}

bool EffectSettings::enabled() const
{
    return m_enabled;
}

void EffectSettings::setBlurRadius(float value)
{
    if (qFuzzyCompare(m_blurRadius, value)) {
        return;
    }

    m_blurRadius = value;
    emit blurRadiusChanged();
    emit settingsChanged();
}

void EffectSettings::setOpacity(float value)
{
    if (qFuzzyCompare(m_opacity, value)) {
        return;
    }

    m_opacity = value;
    emit opacityChanged();
    emit settingsChanged();
}

void EffectSettings::setEnabled(bool value)
{
    if (m_enabled == value) {
        return;
    }

    m_enabled = value;
    emit enabledChanged();
    emit settingsChanged();
}

void EffectSettings::reset()
{
    bool changed = false;

    if (!qFuzzyCompare(m_blurRadius, 10.0f)) {
        m_blurRadius = 10.0f;
        emit blurRadiusChanged();
        changed = true;
    }

    if (!qFuzzyCompare(m_opacity, 1.0f)) {
        m_opacity = 1.0f;
        emit opacityChanged();
        changed = true;
    }

    if (m_enabled != true) {
        m_enabled = true;
        emit enabledChanged();
        changed = true;
    }

    if (changed) {
        emit settingsChanged();
    }
}

QString EffectSettings::imagePath() const
{
    return m_imagePath;
}

void EffectSettings::setImagePath(const QString& path)
{
    if (m_imagePath == path) {
        return;
    }

    m_imagePath = path;
    emit imagePathChanged();
    emit settingsChanged();
}

