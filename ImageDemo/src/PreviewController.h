#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QImage>
#include <QString>

class EffectSettings;

struct LoadResult
{
    bool ok = false;
    QString filePath;
    QString error;
    QImage image;
};

struct ExportResult
{
    bool ok = false;
    QString filePath;
    QString error;
};

class PreviewController : public QObject
{
    Q_OBJECT

public:
    explicit PreviewController(EffectSettings* settings, QObject* parent = nullptr);
    ~PreviewController() override = default;

public slots:
    void openImageFile(const QString& filePath);
    void exportImageFile(const QString& filePath);
    void onSettingsChanged();

signals:
    void busyChanged(bool busy);
    void statusMessageChanged(const QString& message);
    void imageLoaded(const QString& filePath, const QImage& image);
    void imageLoadFailed(const QString& errorMessage);
    void exportFinished(const QString& filePath);
    void exportFailed(const QString& errorMessage);

private:
    EffectSettings* m_settings = nullptr;
    QImage m_currentImage;
    QFutureWatcher<LoadResult> m_loadWatcher;
    QFutureWatcher<ExportResult> m_exportWatcher;
};