#pragma once

#include <QMainWindow>
#include <QHash>
#include <QJsonArray>

class EffectSettings;
class PreviewController;
class QQuickWidget;
class QLabel;
class QSlider;
class QCheckBox;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QNetworkAccessManager;
class PhotoSyncClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    // Cloud sync helpers
    void setupCloudSync();
    void requestCloudThumbnail(const QString& photoId, const QString& imageUrl, QListWidgetItem* item);
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