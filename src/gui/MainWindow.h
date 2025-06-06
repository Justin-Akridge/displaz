// Copyright 2015, Christopher J. Foster and the other displaz contributors.
// Use of this code is governed by the BSD-style license found in LICENSE.txt

#pragma once

#include <QDir>
#include <QMainWindow>
#include <QSettings>
#include <QInputDialog>
#include <QListWidget>
#include <Eigen/Dense>
#include <QWebEngineView>
#include "PointArray.h"


#include <memory>

class QActionGroup;
class QLocalServer;
class QSignalMapper;
class QPlainTextEdit;
class QProgressBar;
class QModelIndex;
class QGLFormat;

class HelpDialog;
class View3D;
class ShaderEditor;
class LogViewer;
class GeometryCollection;
class IpcChannel;
class FileLoader;
class HookManager;


//------------------------------------------------------------------------------
/// Main window for point cloud viewer application
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(const QGLFormat& format);

        /// Hint at an appropriate size
        QSize sizeHint() const;

        /// Return file loader object
        FileLoader& fileLoader() { return *m_fileLoader; }

        /// Start server for interprocess communication
        ///
        /// Listens on local socket `socketName` for incoming connections.  Any
        /// socket previously in use is deleted.
        void startIpcServer(const QString& socketName);

    public slots:
        void handleMessage(QByteArray message);
        void openShaderFile(const QString& shaderFileName);
        QByteArray hookPayload(QByteArray payload);

        void showMessage(const QString& message, int timeout = 10000);
        void clearMessage();

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dropEvent(QDropEvent *event);
        void closeEvent(QCloseEvent *event) override;

    private slots:
        void loadPoles();
        void fullScreen();
        void openFiles();
        void openRecent();
        void addFiles();
        void updateRecentFiles();
        void openShaderFile();
        void saveShaderFile();
        void compileShaderFile();
        void reloadFiles();
        void reloadFile(const QModelIndex& index);
        void screenShot();
        void helpDialog();
        void aboutDialog();
        void setBackground(const QString& name);
        void chooseBackground();
        void updateTitle();
        void loadStepStarted(const QString& description);
        void loadStepComplete();
        void geometryRowsInserted(const QModelIndex& parent, int first, int last);
        void handleIpcConnection();

    private:
        std::vector<Eigen::Vector3d> polePositions;
        void readSettings();
        void writeSettings();

    private:
        // Gui objects
        QProgressBar* m_progressBar = nullptr;
        View3D* m_pointView = nullptr;
        ShaderEditor* m_shaderEditor = nullptr;
        HelpDialog* m_helpDialog = nullptr;
        LogViewer* m_logTextView = nullptr;

        // Gui state
        QString m_currShaderFileName;
        QSettings m_settings;

        QStringList m_recent;
        const qsizetype m_recentLimit = 15;
        QMenu* m_recentMenu = nullptr;

        // Actions
        QAction* m_open = nullptr;
        QAction* m_screenShot = nullptr;
        QAction* m_quit = nullptr;
        QAction* m_quitGeneric = nullptr;
        QAction* m_fullScreen = nullptr;
        QAction* m_trackBall = nullptr;

        //enviroment actions
        QAction* m_loadPoles = nullptr;
        QAction* m_loadMidspans = nullptr;

        void refreshPoleList();
        QWidget* createPoleListItem(const QString& labelText, int index, std::function<void(int)> onDelete);
        QWidget* createPoleEditorWidget();
        // Dock Widgets
        QWebEngineView* m_webView;
        QListWidget* m_poleListWidget;  // Scrollable list of poles
        QDockWidget* m_dockShaderEditor = nullptr;
        QDockWidget* m_dockShaderParameters = nullptr;
        QDockWidget* m_dockDataSet = nullptr;
        QDockWidget* m_dockLog = nullptr;

        // For full-screen toggle
        bool m_dockShaderEditorVisible = false;
        bool m_dockShaderParametersVisible = false;
        bool m_dockDataSetVisible = false;
        bool m_dockLogVisible = false;

        // File loader (slots run on separate thread)
        FileLoader* m_fileLoader;
        /// Maximum desired number of points to load
        size_t m_maxPointCount;
        // Currently loaded geometry
        GeometryCollection* m_geometries = nullptr;

        // Interprocess communication
        QLocalServer* m_ipcServer;

        // Custom event registration for dynamic hooks
        HookManager* m_hookManager;
};
