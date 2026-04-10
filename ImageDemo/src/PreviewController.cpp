#include "PreviewController.h"
#include "EffectSettings.h"
#include <algorithm>

#include <QtConcurrent>
#include <QImageReader>
#include <QThread>

namespace {

int clampInt(int v, int lo, int hi)
{
    return std::max(lo, std::min(v, hi));
}

QImage applyBoxBlur(const QImage& input, int radius)
{
    if (input.isNull() || radius <= 0) {
        return input;
    }

    QImage src = input.convertToFormat(QImage::Format_ARGB32);
    QImage dst(src.size(), QImage::Format_ARGB32);

    const int w = src.width();
    const int h = src.height();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;
            int count = 0;

            for (int ky = -radius; ky <= radius; ++ky) {
                const int sy = clampInt(y + ky, 0, h - 1);

                for (int kx = -radius; kx <= radius; ++kx) {
                    const int sx = clampInt(x + kx, 0, w - 1);
                    const QColor c = QColor::fromRgba(src.pixel(sx, sy));

                    r += c.red();
                    g += c.green();
                    b += c.blue();
                    a += c.alpha();
                    ++count;
                }
            }

            dst.setPixelColor(x, y, QColor(r / count, g / count, b / count, a / count));
        }
    }

    return dst;
}

QImage applyOpacityToImage(const QImage& input, float opacity)
{
    if (input.isNull()) {
        return input;
    }

    opacity = std::max(0.0f, std::min(opacity, 1.0f));

    QImage src = input.convertToFormat(QImage::Format_ARGB32);
    QImage dst(src.size(), QImage::Format_ARGB32);

    const int w = src.width();
    const int h = src.height();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            QColor c = QColor::fromRgba(src.pixel(x, y));
            c.setAlpha(static_cast<int>(c.alpha() * opacity));
            dst.setPixelColor(x, y, c);
        }
    }

    return dst;
}

QImage buildExportImage(const QImage& source,
                        bool effectEnabled,
                        float blurRadius,
                        float effectOpacity)
{
    if (source.isNull()) {
        return source;
    }

    QImage result = source.convertToFormat(QImage::Format_ARGB32);

    if (effectEnabled) {
        const int cpuRadius = std::max(0, static_cast<int>(blurRadius / 8.0f));
        result = applyBoxBlur(result, cpuRadius);
    }

    result = applyOpacityToImage(result, effectOpacity);
    return result;
}

} // namespace

PreviewController::PreviewController(EffectSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
{
    connect(m_settings, &EffectSettings::settingsChanged,
            this, &PreviewController::onSettingsChanged);

    connect(&m_loadWatcher, &QFutureWatcher<LoadResult>::finished, this, [this]() {
        const LoadResult result = m_loadWatcher.result();

        emit busyChanged(false);

        if (!result.ok) {
            emit statusMessageChanged("Open failed");
            emit imageLoadFailed(result.error);
            return;
        }

        m_currentImage = result.image;
        m_settings->setImagePath(result.filePath);

        emit statusMessageChanged(QString("Loaded %1").arg(result.filePath));
        emit imageLoaded(result.filePath, result.image);
    });

    connect(&m_exportWatcher, &QFutureWatcher<ExportResult>::finished, this, [this]() {
        const ExportResult result = m_exportWatcher.result();

        emit busyChanged(false);

        if (!result.ok) {
            emit statusMessageChanged("Export failed");
            emit exportFailed(result.error);
            return;
        }

        emit statusMessageChanged(QString("Exported %1").arg(result.filePath));
        emit exportFinished(result.filePath);
    });
}

void PreviewController::onSettingsChanged()
{
    emit statusMessageChanged(
        QString("Preview updated | blur=%1 opacity=%2 enabled=%3")
            .arg(m_settings->blurRadius())
            .arg(m_settings->opacity())
            .arg(m_settings->enabled() ? "true" : "false"));
}

void PreviewController::openImageFile(const QString& filePath)
{
    if (filePath.isEmpty()) {
        return;
    }

    if (m_loadWatcher.isRunning() || m_exportWatcher.isRunning()) {
        emit statusMessageChanged("Already busy...");
        return;
    }

    emit busyChanged(true);
    emit statusMessageChanged(QString("Opening %1 ...").arg(filePath));

    auto future = QtConcurrent::run([filePath]() -> LoadResult {
        LoadResult result;
        result.filePath = filePath;

        QImageReader reader(filePath);
        reader.setAutoTransform(true);

        const QImage image = reader.read();
        if (image.isNull()) {
            result.ok = false;
            result.error = QString("ERROR: failed to read image: %1")
                               .arg(reader.errorString());
            return result;
        }

        result.ok = true;
        result.image = image;
        return result;
    });

    m_loadWatcher.setFuture(future);
}

void PreviewController::exportImageFile(const QString& filePath)
{
    if (filePath.isEmpty()) {
        return;
    }

    if (m_currentImage.isNull()) {
        emit exportFailed("ERROR: no image loaded");
        emit statusMessageChanged("No image loaded");
        return;
    }

    if (m_loadWatcher.isRunning() || m_exportWatcher.isRunning()) {
        emit statusMessageChanged("Already busy...");
        return;
    }

    emit busyChanged(true);
    emit statusMessageChanged(QString("Exporting %1 ...").arg(filePath));

    const QImage imageToSave = m_currentImage;
    const bool effectEnabled = m_settings->enabled();
    const float blurRadius = m_settings->blurRadius();
    const float effectOpacity = m_settings->opacity();

    auto future = QtConcurrent::run([filePath, imageToSave, effectEnabled, blurRadius, effectOpacity]() -> ExportResult {
        ExportResult result;
        result.filePath = filePath;

        const QImage processed = buildExportImage(
            imageToSave,
            effectEnabled,
            blurRadius,
            effectOpacity);

        if (processed.isNull()) {
            result.ok = false;
            result.error = "ERROR: processed image is null";
            return result;
        }

        if (!processed.save(filePath)) {
            result.ok = false;
            result.error = QString("ERROR: failed to save image to %1").arg(filePath);
            return result;
        }

        result.ok = true;
        return result;
    });

    m_exportWatcher.setFuture(future);
}