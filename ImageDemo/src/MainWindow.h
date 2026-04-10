#pragma once

#include <QMainWindow>
#include <QHash>
#include <QJsonArray>
#include <functional>

class EffectSettings;
class PreviewController;
class QQuickWidget;
class QQuickItem;
class QLabel;
class QSlider;
class QCheckBox;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QNetworkAccessManager;
class PhotoSyncClient;
class QByteArray;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    enum class ExportChoice
    {
        Cancel,
        GpuOnly,
        CpuOnly,
        Both
    };

    // Cloud sync helpers
    void setupCloudSync();
    void autoLoadFirstCloudImage();
    void requestCloudThumbnail(const QString& photoId, const QString& imageUrl, QListWidgetItem* item);
    void loadCloudImageFromUrl(const QString& imageUrl, const QString& displayName);
    void downloadImageBytes(QNetworkAccessManager* net,
                            const QString& imageUrl,
                            std::function<void(const QByteArray&)> onSuccess,
                            std::function<void(const QString&)> onFailure);
    void createCloudGalleryDock();
    void populateCloudGallery(const QJsonArray& photos);
    void onCloudPhotoActivated(QListWidgetItem* item);

    // Core UI setup
    void createMenus();
    void createToolbar();
    void createDockPanels();
    void createPreview();
    void wireUi();
    void updateUiState();

    // Export helpers
    ExportChoice askExportChoice() const;
    void exportCurrentPreviewGpu(const QString& filePath);
    QQuickItem* findQuickItem(const char* objectName) const;

    // Cloud state
    QNetworkAccessManager* m_thumbNet = nullptr;
    QNetworkAccessManager* m_imageNet = nullptr;
    QPushButton* m_syncButton = nullptr;
    PhotoSyncClient* m_syncClient = nullptr;
    QListWidget* m_cloudList = nullptr;
    QHash<QString, QString> m_cloudItemUrlById;

    // Main actions/state
    QAction* m_openAction = nullptr;
    QAction* m_exportAction = nullptr;

    bool m_isBusy = false;
    bool m_hasImageLoaded = false;

    // App model/controllers
    EffectSettings* m_settings = nullptr;
    PreviewController* m_controller = nullptr;

    // Preview widgets
    QQuickWidget* m_quickWidget = nullptr;
    QLabel* m_statusLabel = nullptr;

    // Effect controls
    QSlider* m_blurSlider = nullptr;
    QSlider* m_opacitySlider = nullptr;
    QCheckBox* m_enableCheck = nullptr;
    QPushButton* m_resetButton = nullptr;
    QPushButton* m_exportButton = nullptr;

    QLabel* m_blurValueLabel = nullptr;
    QLabel* m_opacityValueLabel = nullptr;

private slots:
    void onOpenImage();
    void onExportImage();
};