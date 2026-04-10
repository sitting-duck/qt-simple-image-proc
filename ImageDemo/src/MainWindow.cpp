QQuickItem* MainWindow::findQuickItem(const char* objectName) const
{
    if (!m_quickWidget) {
        return nullptr;
    }

    QObject* root = m_quickWidget->rootObject();
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

    QObject* root = m_quickWidget->rootObject();
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

    const int imageStatus = sourceItem->property("status").toInt();
    if (imageStatus != 1) {
        QMessageBox::warning(this, "Export Failed", "Source image is not ready yet.");
        return;
    }

    const int sourcePixelWidth = qMax(1, sourceItem->property("implicitWidth").toInt());
    const int sourcePixelHeight = qMax(1, sourceItem->property("implicitHeight").toInt());
    const QSize targetSize(sourcePixelWidth, sourcePixelHeight);

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