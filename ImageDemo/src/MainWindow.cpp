#include "MainWindow.h"
#include "EffectSettings.h"
#include "PreviewController.h"
#include "PhotoSyncClient.h"

#include <QQuickWidget>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QStatusBar>
#include <QSharedPointer>
#include <QAction>
#include <QByteArray>
#include <QCheckBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QIcon>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QPushButton>
#include <QQuickWidget>
#include <QQmlContext>
#include <QSize>
#include <QSlider>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new EffectSettings(this))
    , m_controller(new PreviewController(m_settings, this))
{
    createMenus();
    createToolbar();
    createDockPanels();
    createCloudGalleryDock();

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

    createPreview();
    wireUi();

    updateUiState();
    statusBar()->showMessage("Ready");

    setupCloudSync();
}

// Cloud sync setup

void MainWindow::setupCloudSync()
{
    m_syncClient = new PhotoSyncClient(this);
    m_thumbNet = new QNetworkAccessManager(this);
    m_imageNet = new QNetworkAccessManager(this);

    m_syncClient->setBaseUrl("https://npuctj4dl3.execute-api.us-east-1.amazonaws.com/prod");

    connect(m_syncClient, &PhotoSyncClient::photosReceived,
            this, [this](const QJsonArray& photos) {
                populateCloudGallery(photos);

                if (m_syncButton) {
                    m_syncButton->setEnabled(true);
                }

                autoLoadFirstCloudImage();

                statusBar()->showMessage(
                    QString("Cloud gallery synced: %1 photo(s)").arg(photos.size()),
                    3000);
            });

    connect(m_syncClient, &PhotoSyncClient::requestFailed,
            this, [this](const QString& err) {
                if (m_syncButton) {
                    m_syncButton->setEnabled(true);
                }

                QMessageBox::warning(this, "Cloud Sync Failed", err);
                statusBar()->showMessage("Cloud sync failed", 3000);
            });

    m_syncClient->fetchPhotos();
}

void MainWindow::autoLoadFirstCloudImage()
{
    if (!m_cloudList || m_cloudList->count() == 0) {
        return;
    }

    auto* firstItem = m_cloudList->item(0);
    if (!firstItem) {
        return;
    }

    m_cloudList->setCurrentItem(firstItem);
    onCloudPhotoActivated(firstItem);
}

// Shared cloud download helper

void MainWindow::downloadImageBytes(QNetworkAccessManager* net,
                                    const QString& imageUrl,
                                    std::function<void(const QByteArray&)> onSuccess,
                                    std::function<void(const QString&)> onFailure)
{
    if (!net) {
        onFailure("Network manager is not available.");
        return;
    }

    if (imageUrl.isEmpty()) {
        onFailure("Missing image URL.");
        return;
    }

    QNetworkRequest request{QUrl(imageUrl)};
    QNetworkReply* reply = net->get(request);

    connect(reply, &QNetworkReply::finished, this, [reply, onSuccess = std::move(onSuccess), onFailure = std::move(onFailure)]() mutable {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            onFailure(reply->errorString());
            return;
        }

        onSuccess(reply->readAll());
    });
}

// Cloud gallery actions

void MainWindow::onCloudPhotoActivated(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    const QString photoId = item->data(Qt::UserRole).toString();
    const QString imageUrl = m_cloudItemUrlById.value(photoId);

    if (imageUrl.isEmpty()) {
        QMessageBox::warning(this, "Load Failed", "Missing image URL for selected photo.");
        return;
    }

    loadCloudImageFromUrl(imageUrl, item->text());
}

void MainWindow::loadCloudImageFromUrl(const QString& imageUrl, const QString& displayName)
{
    statusBar()->showMessage("Downloading cloud image...");

    downloadImageBytes(
        m_imageNet,
        imageUrl,
        [this, imageUrl, displayName](const QByteArray& bytes) {
            QImage image;
            if (!image.loadFromData(bytes)) {
                QMessageBox::warning(this, "Load Failed", "Downloaded data was not a valid image.");
                return;
            }

            m_controller->openImage(image, imageUrl);
            m_hasImageLoaded = true;
            updateUiState();
            statusBar()->showMessage(QString("Loaded cloud image: %1").arg(displayName), 3000);
        },
        [this](const QString& error) {
            QMessageBox::warning(this, "Load Failed", error);
        });
}

void MainWindow::populateCloudGallery(const QJsonArray& photos)
{
    if (!m_cloudList) {
        return;
    }

    m_cloudList->clear();
    m_cloudItemUrlById.clear();

    for (const QJsonValue& value : photos) {
        const QJsonObject obj = value.toObject();

        const QString photoId = obj.value("photoId").toString();
        const QString fileName = obj.value("fileName").toString().trimmed();
        const QString imageUrl = obj.value("imageUrl").toString();

        if (photoId.isEmpty() || imageUrl.isEmpty()) {
            continue;
        }

        auto* item = new QListWidgetItem(fileName.isEmpty() ? photoId : fileName);
        item->setData(Qt::UserRole, photoId);
        item->setToolTip(fileName);
        item->setSizeHint(QSize(item->sizeHint().width(), 84));

        m_cloudList->addItem(item);
        m_cloudItemUrlById.insert(photoId, imageUrl);

        requestCloudThumbnail(photoId, imageUrl, item);
    }
}

void MainWindow::createCloudGalleryDock()
{
    auto* cloudDock = new QDockWidget("Cloud Gallery", this);
    cloudDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    auto* container = new QWidget(cloudDock);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    m_syncButton = new QPushButton("Sync", container);
    m_cloudList = new QListWidget(container);
    m_cloudList->setSelectionMode(QAbstractItemView::SingleSelection);

    m_cloudList->setViewMode(QListView::IconMode);
    m_cloudList->setMovement(QListView::Static);
    m_cloudList->setGridSize(QSize(110, 110));
    m_cloudList->setWordWrap(true);

    layout->addWidget(m_syncButton);
    layout->addWidget(m_cloudList);

    connect(m_syncButton, &QPushButton::clicked, this, [this]() {
        if (!m_syncClient) {
            return;
        }

        statusBar()->showMessage("Syncing cloud gallery...");
        m_syncButton->setEnabled(false);
        m_syncClient->fetchPhotos();
    });

    connect(m_cloudList, &QListWidget::itemClicked,
            this, &MainWindow::onCloudPhotoActivated);

    m_cloudList->setIconSize(QSize(72, 72));
    m_cloudList->setResizeMode(QListView::Adjust);
    m_cloudList->setUniformItemSizes(false);
    m_cloudList->setSpacing(6);
    m_cloudList->setStyleSheet("QListWidget::item { height: 84px; }");

    cloudDock->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea, cloudDock);
}

void MainWindow::requestCloudThumbnail(const QString& photoId,
                                       const QString& imageUrl,
                                       QListWidgetItem* item)
{
    if (!m_thumbNet || !item) {
        return;
    }

    Q_UNUSED(photoId);

    downloadImageBytes(
        m_thumbNet,
        imageUrl,
        [this, item](const QByteArray& bytes) {
            if (!m_cloudList) {
                return;
            }

            QImage image;
            if (!image.loadFromData(bytes)) {
                return;
            }

            const QImage thumb = image.scaled(
                72, 72,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            item->setIcon(QIcon(QPixmap::fromImage(thumb)));
        },
        [](const QString&) {
        });
}

// Main window UI construction

void MainWindow::createMenus()
{
    auto* fileMenu = menuBar()->addMenu("&File");

    m_openAction = new QAction("Open...", this);
    m_exportAction = new QAction("Export As...", this);
    auto* quitAction = new QAction("Quit", this);

    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenImage);
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::onExportImage);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    auto* editMenu = menuBar()->addMenu("&Edit");
    auto* resetAction = new QAction("Reset Effects", this);

    connect(resetAction, &QAction::triggered, m_settings, &EffectSettings::reset);
    editMenu->addAction(resetAction);
}

void MainWindow::createToolbar()
{
    auto* toolbar = addToolBar("Main");
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_exportAction);
}

void MainWindow::createDockPanels()
{
    auto* controlsDock = new QDockWidget("Effect Controls", this);
    controlsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    controlsDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    auto* controlsWidget = new QWidget(controlsDock);
    controlsWidget->setMinimumWidth(260);

    auto* layout = new QVBoxLayout(controlsWidget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignTop);
    formLayout->setSpacing(10);

    m_blurSlider = new QSlider(Qt::Horizontal, controlsWidget);
    m_blurSlider->setRange(0, 100);
    m_blurSlider->setValue(10);

    m_blurValueLabel = new QLabel("10.0", controlsWidget);
    m_blurValueLabel->setMinimumWidth(40);
    m_blurValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto* blurRow = new QWidget(controlsWidget);
    auto* blurLayout = new QHBoxLayout(blurRow);
    blurLayout->setContentsMargins(0, 0, 0, 0);
    blurLayout->addWidget(m_blurSlider);
    blurLayout->addWidget(m_blurValueLabel);

    m_opacitySlider = new QSlider(Qt::Horizontal, controlsWidget);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);

    m_opacityValueLabel = new QLabel("1.00", controlsWidget);
    m_opacityValueLabel->setMinimumWidth(40);
    m_opacityValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto* opacityRow = new QWidget(controlsWidget);
    auto* opacityLayout = new QHBoxLayout(opacityRow);
    opacityLayout->setContentsMargins(0, 0, 0, 0);
    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityValueLabel);

    m_enableCheck = new QCheckBox("Enable Blur", controlsWidget);
    m_enableCheck->setChecked(true);

    m_resetButton = new QPushButton("Reset", controlsWidget);
    m_exportButton = new QPushButton("Export", controlsWidget);

    formLayout->addRow("Blur Radius", blurRow);
    formLayout->addRow("Opacity", opacityRow);

    layout->addLayout(formLayout);
    layout->addWidget(m_enableCheck);
    layout->addWidget(m_resetButton);
    layout->addWidget(m_exportButton);
    layout->addStretch();

    controlsDock->setWidget(controlsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, controlsDock);
}

void MainWindow::createPreview()
{
    m_quickWidget = new QQuickWidget(this);
    m_quickWidget->rootContext()->setContextProperty("effectSettings", m_settings);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->setSource(QUrl("qrc:/ImageDemo/qml/PreviewView.qml"));

    setCentralWidget(m_quickWidget);
}

// Preview and effect wiring

void MainWindow::wireUi()
{
    connect(m_blurSlider, &QSlider::valueChanged, this, [this](int value) {
        m_settings->setBlurRadius(static_cast<float>(value));
        m_blurValueLabel->setText(QString::number(value, 'f', 1));
    });

    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int value) {
        const float opacity = static_cast<float>(value) / 100.0f;
        m_settings->setOpacity(opacity);
        m_opacityValueLabel->setText(QString::number(opacity, 'f', 2));
    });

    connect(m_enableCheck, &QCheckBox::toggled,
            m_settings, &EffectSettings::setEnabled);

    connect(m_resetButton, &QPushButton::clicked,
            m_settings, &EffectSettings::reset);

    connect(m_exportButton, &QPushButton::clicked,
            this, &MainWindow::onExportImage);

    connect(m_settings, &EffectSettings::blurRadiusChanged, this, [this]() {
        m_blurSlider->blockSignals(true);
        m_blurSlider->setValue(static_cast<int>(m_settings->blurRadius()));
        m_blurSlider->blockSignals(false);

        m_blurValueLabel->setText(
            QString::number(m_settings->blurRadius(), 'f', 1));
    });

    connect(m_settings, &EffectSettings::opacityChanged, this, [this]() {
        m_opacitySlider->blockSignals(true);
        m_opacitySlider->setValue(static_cast<int>(m_settings->opacity() * 100.0f));
        m_opacitySlider->blockSignals(false);

        m_opacityValueLabel->setText(
            QString::number(m_settings->opacity(), 'f', 2));
    });

    connect(m_settings, &EffectSettings::enabledChanged, this, [this]() {
        m_enableCheck->blockSignals(true);
        m_enableCheck->setChecked(m_settings->enabled());
        m_enableCheck->blockSignals(false);
    });

    connect(m_controller, &PreviewController::statusMessageChanged,
            this, [this](const QString& message) {
                statusBar()->showMessage(message);
            });

    connect(m_controller, &PreviewController::busyChanged,
            this, [this](bool busy) {
                m_isBusy = busy;
                updateUiState();
            });

    connect(m_controller, &PreviewController::imageLoaded,
            this, [this](const QString&, const QImage&) {
                m_hasImageLoaded = true;
                updateUiState();
            });

    connect(m_controller, &PreviewController::imageLoadFailed,
            this, [this](const QString& error) {
                QMessageBox::warning(this, "Open Failed", error);
            });

    connect(m_controller, &PreviewController::exportFinished,
            this, [this](const QString& path) {
                statusBar()->showMessage(QString("Export finished: %1").arg(path), 5000);
            });

    connect(m_controller, &PreviewController::exportFailed,
            this, [this](const QString& error) {
                QMessageBox::warning(this, "Export Failed", error);
            });
}

// File actions

void MainWindow::onOpenImage()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Image",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp)");

    if (filePath.isEmpty()) {
        return;
    }

    m_controller->openImageFile(filePath);
}

void MainWindow::onExportImage()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Image",
        "export.png",
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)");

    if (filePath.isEmpty()) {
        return;
    }

    exportCurrentPreviewGpu(filePath);
}

void MainWindow::updateUiState()
{
    const bool canExport = !m_isBusy && m_hasImageLoaded;
    const bool canEditControls = !m_isBusy;

    if (m_exportButton) {
        m_exportButton->setEnabled(canExport);
    }

    if (m_exportAction) {
        m_exportAction->setEnabled(canExport);
    }

    if (m_openAction) {
        m_openAction->setEnabled(!m_isBusy);
    }

    if (m_blurSlider) {
        m_blurSlider->setEnabled(canEditControls);
    }

    if (m_opacitySlider) {
        m_opacitySlider->setEnabled(canEditControls);
    }

    if (m_enableCheck) {
        m_enableCheck->setEnabled(canEditControls);
    }

    if (m_resetButton) {
        m_resetButton->setEnabled(canEditControls);
    }
}

QQuickItem* MainWindow::findQuickItem(const char* objectName) const
{
    if (!m_quickWidget) {
        return nullptr;
    }

    QQuickItem* root = m_quickWidget->rootObject();
    if (!root) {
        return nullptr;
    }

    QObject* child = root->findChild<QObject*>(QString::fromUtf8(objectName), Qt::FindChildrenRecursively);
    return qobject_cast<QQuickItem*>(child);
}

void MainWindow::exportCurrentPreviewGpu(const QString& filePath)
{
    if (!m_quickWidget) {
        QMessageBox::warning(this, "Export Failed", "Preview widget is not available.");
        return;
    }

    QQuickItem* root = m_quickWidget->rootObject();
    if (!root) {
        QMessageBox::warning(this, "Export Failed", "QML root object is not ready.");
        return;
    }

    auto* previewItem = findQuickItem("previewEffect");
    auto* sourceItem = findQuickItem("sourceImage");

    if (!previewItem || !sourceItem) {
        QString detail = "Preview items not found.\n\n";
        detail += QString("rootObject class: %1\n").arg(root->metaObject()->className());
        detail += QString("previewEffect found: %1\n").arg(previewItem ? "yes" : "no");
        detail += QString("sourceImage found: %1\n").arg(sourceItem ? "yes" : "no");
        detail += "\nMake sure PreviewView.qml contains:\n";
        detail += "objectName: \"previewEffect\"\n";
        detail += "objectName: \"sourceImage\"";

        QMessageBox::warning(this, "Export Failed", detail);
        return;
    }

    QSize sourceSize = sourceItem->property("sourceSize").toSize();
    if (!sourceSize.isValid() || sourceSize.isEmpty()) {
        sourceSize = QSize(
            qMax(1, static_cast<int>(sourceItem->width())),
            qMax(1, static_cast<int>(sourceItem->height()))
            );
    }

    const QSize targetSize(
        qMax(1, sourceSize.width()),
        qMax(1, sourceSize.height())
        );

    m_isBusy = true;
    updateUiState();
    statusBar()->showMessage(QString("Exporting GPU render to %1 ...").arg(filePath));

    const QSharedPointer<QQuickItemGrabResult> grabResult = previewItem->grabToImage(targetSize);

    if (!grabResult) {
        m_isBusy = false;
        updateUiState();
        QMessageBox::warning(this, "Export Failed", "Failed to start GPU export.");
        return;
    }

    connect(grabResult.data(), &QQuickItemGrabResult::ready, this, [this, grabResult, filePath]() {
        m_isBusy = false;
        updateUiState();

        const QImage image = grabResult->image();
        if (image.isNull()) {
            QMessageBox::warning(this, "Export Failed", "GPU export produced an empty image.");
            statusBar()->showMessage("GPU export failed", 5000);
            return;
        }

        if (!image.save(filePath)) {
            QMessageBox::warning(this, "Export Failed", QString("Failed to save image to %1").arg(filePath));
            statusBar()->showMessage("GPU export failed", 5000);
            return;
        }

        statusBar()->showMessage(QString("GPU export finished: %1").arg(filePath), 5000);
    });
}