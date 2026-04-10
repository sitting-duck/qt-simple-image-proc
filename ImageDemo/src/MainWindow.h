#pragma once

#include <QMainWindow>

class EffectSettings;
class PreviewController;
class QQuickWidget;
class QLabel;
class QSlider;
class QCheckBox;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    QAction* m_openAction = nullptr;
    QAction* m_exportAction = nullptr;

    bool m_isBusy = false;
    bool m_hasImageLoaded = false;

    void createMenus();
    void createToolbar();
    void createDockPanels();
    void createPreview();
    void wireUi();
    void updateUiState();

    EffectSettings* m_settings = nullptr;
    PreviewController* m_controller = nullptr;

    QQuickWidget* m_quickWidget = nullptr;
    QLabel* m_statusLabel = nullptr;

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
