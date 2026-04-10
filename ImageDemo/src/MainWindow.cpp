#include "MainWindow.h"
#include "EffectSettings.h"
#include "PreviewController.h"
#include "PhotoSyncClient.h"
#include <QDebug>
#include <QAction>
#include <QCheckBox>
#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QQuickWidget>
#include <QSlider>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new EffectSettings(this))
    , m_controller(new PreviewController(m_settings, this))
{
    createMenus();
    createToolbar();
    createDockPanels();
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    createPreview();
    wireUi();

    statusBar()->showMessage("Ready");

    updateUiState();
    statusBar()->showMessage("Ready");

    m_syncClient = new PhotoSyncClient(this);

    m_syncClient->setBaseUrl("https://npuctj4dl3.execute-api.us-east-1.amazonaws.com/prod");

    // Handle success
    connect(m_syncClient, &PhotoSyncClient::photosReceived,
            this, [](const QJsonArray& photos) {
                qDebug() << "Photos received:" << photos.size();

                for (const QJsonValue& value : photos) {
                    const QJsonObject obj = value.toObject();

                    const QString photoId = obj.value("photoId").toString();
                    const QString fileName = obj.value("fileName").toString();
                    const QString imageUrl = obj.value("imageUrl").toString();
                    const QString s3Key = obj.value("s3Key").toString();
                    const qint64 sizeBytes = static_cast<qint64>(obj.value("sizeBytes").toDouble());

                    qDebug() << "photoId:" << photoId;
                    qDebug() << "fileName:" << fileName;
                    qDebug() << "imageUrl:" << imageUrl;
                    qDebug() << "s3Key:" << s3Key;
                    qDebug() << "sizeBytes:" << sizeBytes;
                    qDebug() << "------------------------";
                }
            });

    // Handle errors
    connect(m_syncClient, &PhotoSyncClient::requestFailed,
            this, [](const QString& err) {
                qDebug() << "Photo fetch failed:" << err;
            });

    // Kick it off
    m_syncClient->fetchPhotos();
}

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

    // Blur slider + value
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

    // Opacity slider + value
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

    m_controller->exportImageFile(filePath);
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
