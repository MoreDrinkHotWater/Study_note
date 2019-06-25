//
// Created by zhihui on 3/27/19.
//

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QIcon>
#include <QApplication>
#include <QTreeWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QAction>
#include <QDir>
#include <QCloseEvent>
#include <QProgressDialog>
#include <QDebug>

#include "MainWindow.h"
#include "../OSGWidgets/OSGWidget.h"
#include "OpenFileDialog.h"
#include "GenerateOctreeWorker.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          osgWidget(new OSGWidget(this)),
                                          dockWidget(new QDockWidget(this)),
                                          treeWidget(new QTreeWidget(this)),
                                          pMenuBar(menuBar()),
                                          pToolBar(addToolBar("Tools")),
                                          pStatusBar(statusBar()),
                                          clickedEditorAction(nullptr),
                                          clickedColorAction(nullptr),
                                          openFileAction(new QAction(this)),
                                          saveFileAction(new QAction(this)),
                                          saveAsFileAction(new QAction(this)),
                                          closeCurrentFileAction(new QAction(this)),
                                          quitAppActionAction(new QAction(this)),
                                          drawLineAction(new QAction(this)),
                                          modifyLineAction(new QAction(this)),
                                          deleteLineAction(new QAction(this)),
                                          drawTraceLineAction(new QAction(this)),
                                          modifyTraceLineAction(new QAction(this)),
                                          deleteTraceLineAction(new QAction(this)),
                                          drawPavementAction(new QAction(this)),
                                          deletePavementAction(new QAction(this)),
                                          drawDrivingArrowAction(new QAction(this)),
                                          modifyDrivingArrowAction(new QAction(this)),
                                          deleteDrivingArrowAction(new QAction(this)),
                                          drawTrafficLightsAction(new QAction(this)),
                                          deleteTrafficLightsAction(new QAction(this)),
                                          drawRoadLinesAction(new QAction(this)),
                                          modifyRoadLinesAction(new QAction(this)),
                                          deleteRoadLinesAction(new QAction(this)),
                                          colorByZAction(new QAction(this)),
                                          colorByIntensityAction(new QAction(this)),
                                          colorByTextureAction(new QAction(this)),
                                          helpAction(new QAction(this)),
                                          aboutAction(new QAction(this)),
                                          openFileInfo(),
                                          originalPCDFileName(QString()),
                                          fileExtension(),
                                          hasIntensity(false),
                                          hasRGB(false),
                                          generateOctreeWorker(new GenerateOctreeWorker()),
                                          progressDialog(new QProgressDialog("Reading files...",
                                                                             QString(),
                                                                             0,
                                                                             0,
                                                                             this)) {
    setWindowTitle("HDMaps");

    progressDialog->reset();
    connect(this,
            &MainWindow::startGenerateOctreeDataSignal,
            generateOctreeWorker,
            &GenerateOctreeWorker::generateOctreeData);
    connect(generateOctreeWorker,
            &GenerateOctreeWorker::doneGenerateOctreeDataSignal,
            this,
            &MainWindow::doneGernerateOctreeData);
    generateOctreeWorker->moveToThread(&generateOctreeThread);
    // connect(&generateOctreeThread, &QThread::finished, generateOctreeWorker, &QObject::deleteLater);

    initActions();
    initMenu();
    initToolBar();
    initStatusBar();

    // createDockWidget();

    setCentralWidget(osgWidget);
    osgWidget->init();
}

MainWindow::~MainWindow() {
    generateOctreeThread.quit();
    generateOctreeThread.wait();
    delete generateOctreeWorker;
}

void MainWindow::initActions() {
    openFileAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/file_open.png")));
    openFileAction->setText("&Open...");
    openFileAction->setShortcut(QKeySequence::Open);
    openFileAction->setStatusTip("Open File");
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);

    saveFileAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/file_save.png")));
    saveFileAction->setText("&Save");
    saveFileAction->setShortcut(QKeySequence::Save);
    saveFileAction->setStatusTip("Save File");
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);

    saveAsFileAction->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/images/file_save_as.png")));
    saveAsFileAction->setText("&SaveAS");
    saveAsFileAction->setShortcut(QKeySequence::SaveAs);
    saveAsFileAction->setStatusTip("SaveAS File");
    connect(saveAsFileAction, &QAction::triggered, this, &MainWindow::saveAsFile);

    closeCurrentFileAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/file_close.png")));
    closeCurrentFileAction->setText("&CloseCurrentFile");
    closeCurrentFileAction->setShortcut(QKeySequence::Close);
    closeCurrentFileAction->setStatusTip("Close Current File");
    connect(closeCurrentFileAction, &QAction::triggered, this, &MainWindow::closeCurrentFile);

    quitAppActionAction->setIcon(QIcon::fromTheme("application-exit", QIcon(":/images/appExit.png")));
    quitAppActionAction->setText("&Quit");
    quitAppActionAction->setShortcut(QKeySequence::Quit);
    quitAppActionAction->setStatusTip("Quit");
    connect(quitAppActionAction, &QAction::triggered, this, &MainWindow::quitApp, Qt::QueuedConnection);

    drawLineAction->setIcon(QIcon(":/images/line.png"));
    drawLineAction->setText("Draw Line");
    drawLineAction->setStatusTip("Draw line");
    drawLineAction->setCheckable(true);
    connect(drawLineAction, &QAction::triggered, this, &MainWindow::drawLine);

    modifyLineAction->setIcon(QIcon(":/images/modify.jpeg"));
    modifyLineAction->setText("Modify line");
    modifyLineAction->setStatusTip("Modify line");
    modifyLineAction->setCheckable(true);
    connect(modifyLineAction, &QAction::triggered, this, &MainWindow::modifyLine);

    deleteLineAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/delete.png")));
    deleteLineAction->setText("Delete line");
    deleteLineAction->setStatusTip("Delete line");
    deleteLineAction->setCheckable(true);
    connect(deleteLineAction, &QAction::triggered, this, &MainWindow::deleteLine);

    drawTraceLineAction->setIcon(QIcon(":/images/traceLine.png"));
    drawTraceLineAction->setText("Draw Trace line");
    drawTraceLineAction->setStatusTip("Draw Trace line");
    drawTraceLineAction->setCheckable(true);
    connect(drawTraceLineAction, &QAction::triggered, this, &MainWindow::drawTraceLine);

    modifyTraceLineAction->setIcon(QIcon(":/images/modifyTraceLine.jpeg"));
    modifyTraceLineAction->setText("Modify Trace line");
    modifyTraceLineAction->setStatusTip("Modify Trace line");
    modifyTraceLineAction->setCheckable(true);
    connect(modifyTraceLineAction, &QAction::triggered, this, &MainWindow::modifyTraceLine);

    deleteTraceLineAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/deleteTraceLine.png")));
    deleteTraceLineAction->setText("Delete Trace line");
    deleteTraceLineAction->setStatusTip("Delete Trace line");
    deleteTraceLineAction->setCheckable(true);
    connect(deleteTraceLineAction, &QAction::triggered, this, &MainWindow::deleteTraceLine);

    drawPavementAction->setIcon(QIcon(":/images/pavement.png"));
    drawPavementAction->setText("Draw pavement");
    drawPavementAction->setStatusTip("Draw pavement");
    drawPavementAction->setCheckable(true);
    connect(drawPavementAction, &QAction::triggered, this, &MainWindow::drawPavement);

    deletePavementAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/deletePavement.png")));
    deletePavementAction->setText("Delete pavement");
    deletePavementAction->setStatusTip("Delete pavement");
    deletePavementAction->setCheckable(true);
    connect(deletePavementAction, &QAction::triggered, this, &MainWindow::deletePavement);

    drawDrivingArrowAction->setIcon(QIcon(":/images/drivingArrow.jpeg"));
    drawDrivingArrowAction->setText("Draw drivingArrow");
    drawDrivingArrowAction->setStatusTip("Draw drivingArrow");
    drawDrivingArrowAction->setCheckable(true);
    connect(drawDrivingArrowAction, &QAction::triggered, this, &MainWindow::drawDrivingArrow);

    modifyDrivingArrowAction->setIcon(QIcon(":/images/modifyDrivingArrow.jpeg"));
    modifyDrivingArrowAction->setText("Modify drivingArrow");
    modifyDrivingArrowAction->setStatusTip("Modify drivingArrow");
    modifyDrivingArrowAction->setCheckable(true);
    connect(modifyDrivingArrowAction, &QAction::triggered, this, &MainWindow::modifyDrivingArrow);

    deleteDrivingArrowAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/deleteDrivingArrow.png")));
    deleteDrivingArrowAction->setText("Delete drivingArrow");
    deleteDrivingArrowAction->setStatusTip("Delete drivingArrow");
    deleteDrivingArrowAction->setCheckable(true);
    connect(deleteDrivingArrowAction, &QAction::triggered, this, &MainWindow::deleteDrivingArrow);

    drawTrafficLightsAction->setIcon(QIcon(":/images/drawTrafficLights.jpeg"));
    drawTrafficLightsAction->setText("Draw TrafficLights");
    drawTrafficLightsAction->setStatusTip("Draw TrafficLights");
    drawTrafficLightsAction->setCheckable(true);
    connect(drawTrafficLightsAction, &QAction::triggered, this, &MainWindow::drawTrafficLights);

    deleteTrafficLightsAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/deleteTrafficLights.png")));
    deleteTrafficLightsAction->setText("Delete TrafficLights");
    deleteTrafficLightsAction->setStatusTip("Delete TrafficLights");
    deleteTrafficLightsAction->setCheckable(true);
    connect(deleteTrafficLightsAction, &QAction::triggered, this, &MainWindow::deleteTrafficLights);

    drawRoadLinesAction->setIcon(QIcon(":/images/drawRoadLines.jpeg"));
    drawRoadLinesAction->setText("Draw RoadLines");
    drawRoadLinesAction->setStatusTip("Draw RoadLines");
    drawRoadLinesAction->setCheckable(true);
    connect(drawRoadLinesAction, &QAction::triggered, this, &MainWindow::drawRoadLines);

    modifyRoadLinesAction->setIcon(QIcon(":/images/modifyRoadLines.png"));
    modifyRoadLinesAction->setText("Modify RoadLines");
    modifyRoadLinesAction->setStatusTip("Modify RoadLines");
    modifyRoadLinesAction->setCheckable(true);
    connect(modifyRoadLinesAction, &QAction::triggered, this, &MainWindow::modifyRoadLines);

    deleteRoadLinesAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/deleteRoadLines.png")));
    deleteRoadLinesAction->setText("Delete RoadLines");
    deleteRoadLinesAction->setStatusTip("Delete RoadLines");
    deleteRoadLinesAction->setCheckable(true);
    connect(deleteRoadLinesAction, &QAction::triggered, this, &MainWindow::deleteRoadLines);

    colorByZAction->setIcon(QIcon(":/images/colorByZ.jpeg"));
    colorByZAction->setText("Color By Z");
    colorByZAction->setStatusTip("Color By Z");
    colorByZAction->setCheckable(true);
    connect(colorByZAction, &QAction::triggered, this, &MainWindow::colorByZ);

    colorByIntensityAction->setIcon(QIcon(":/images/colorByIntensity.jpeg"));
    colorByIntensityAction->setText("Color By Intensity");
    colorByIntensityAction->setStatusTip("Color By Intensity");
    colorByIntensityAction->setCheckable(true);
    connect(colorByIntensityAction, &QAction::triggered, this, &MainWindow::colorByIntensity);

    colorByTextureAction->setIcon(QIcon(":/images/colorByTexture.jpeg"));
    colorByTextureAction->setText("Color By Texture");
    colorByTextureAction->setStatusTip("Color By Texture");
    colorByTextureAction->setCheckable(true);
    connect(colorByTextureAction, &QAction::triggered, this, &MainWindow::colorByTexture);

    helpAction->setIcon(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")));
    helpAction->setText("&Help");
    helpAction->setShortcut(QKeySequence::HelpContents);
    helpAction->setStatusTip("Help");
    connect(helpAction, &QAction::triggered, this, &MainWindow::help);

    aboutAction->setIcon(QIcon::fromTheme("help-about", QIcon(":/images/about.png")));
    aboutAction->setText("&About");
    aboutAction->setShortcut(QKeySequence::WhatsThis);
    aboutAction->setStatusTip("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

}

void MainWindow::initMenu() {

    QMenu *menu;

    menu = new QMenu("&File", this);
    menu->addAction(openFileAction);
    menu->addSeparator();

    menu->addAction(closeCurrentFileAction);
    menu->addSeparator();

    menu->addAction(saveFileAction);
    menu->addAction(saveAsFileAction);
    menu->addSeparator();

    menu->addAction(quitAppActionAction);

    pMenuBar->addMenu(menu);

    menu = new QMenu("&Draw", this);
//    menu->addActions({drawLineAction, modifyLineAction, deleteLineAction});
//    menu->addSeparator();

    menu->addActions({drawTraceLineAction, modifyTraceLineAction, deleteTraceLineAction});
    menu->addSeparator();

    menu->addActions({drawPavementAction, deletePavementAction});
    menu->addSeparator();

    menu->addActions({drawDrivingArrowAction, modifyDrivingArrowAction, deleteDrivingArrowAction});
    menu->addSeparator();

    menu->addActions({drawTrafficLightsAction, deleteTrafficLightsAction});
    menu->addSeparator();

    menu->addActions({drawRoadLinesAction, modifyRoadLinesAction, deleteRoadLinesAction});
    menu->addSeparator();

    pMenuBar->addMenu(menu);

    menu = new QMenu("&Render", this);
    menu->addActions({colorByZAction, colorByIntensityAction, colorByTextureAction});

    pMenuBar->addMenu(menu);

    menu = new QMenu("&Help", this);
    menu->addActions({helpAction, aboutAction});

    pMenuBar->addMenu(menu);
}

void MainWindow::initToolBar() {
    pToolBar->addActions({openFileAction, saveFileAction, saveAsFileAction, closeCurrentFileAction});
    pToolBar->addSeparator();
    pToolBar->addActions({drawTraceLineAction, modifyTraceLineAction, deleteTraceLineAction});
    pToolBar->addSeparator();
//    pToolBar->addActions({drawLineAction, modifyLineAction, deleteLineAction});
//    pToolBar->addSeparator();
    pToolBar->addActions({drawPavementAction, deletePavementAction});
    pToolBar->addSeparator();
    pToolBar->addActions({drawDrivingArrowAction, modifyDrivingArrowAction, deleteDrivingArrowAction});
    pToolBar->addSeparator();
    pToolBar->addActions({drawTrafficLightsAction, deleteTrafficLightsAction});
    pToolBar->addSeparator();
    pToolBar->addActions({drawRoadLinesAction, modifyRoadLinesAction, deleteRoadLinesAction});
    pToolBar->addSeparator();
    pToolBar->addActions({colorByZAction, colorByIntensityAction, colorByTextureAction});
}

void MainWindow::initStatusBar() {
    pStatusBar->showMessage("Ready");
}

void MainWindow::createDockWidget() {

    treeWidget->setColumnCount(2);
    treeWidget->setHeaderHidden(true);
    QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget, QStringList({"FirstColumn", "SecondColumn"}));
    item->setExpanded(true);

    dockWidget->setWindowTitle(QStringLiteral("场景数据"));
    dockWidget->setFixedWidth(210);
    dockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setWidget(treeWidget);
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
}

void MainWindow::doneGernerateOctreeData() {
    progressDialog->cancel();
}

void MainWindow::closeEvent(QCloseEvent *e) {
    // 文件未打开
    if (!openFileInfo.fileName().isEmpty()) {
        if (QMessageBox::warning(nullptr,
                                 "Close Current File",
                                 "Save file or not?",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes) == QMessageBox::Yes) {
            saveFile();
        }
    }
    e->accept();
}

void MainWindow::openFile() {

    auto *openFileDialog = new OpenFileDialog();
    connect(openFileDialog, &OpenFileDialog::openFileInfoSignal, this, &MainWindow::openFileInfoSlot);
    openFileDialog->exec();

    // 调试用上面一个，因为是外围build编译，部署发布用下面，数据和可执行文件在同一层
//    QString fileName = QFileDialog::getOpenFileName(this, "Open PointCloud File", "../../pointCloudData",
//                                                    "PointCloud (*.pcd *.txt *.las)");

    // 打开 ./pointCloudData 路径 格式为：*.pcd *.txt *.las 的文件
    /*
     * Open PointCloud File 为文件框的 Title
     * ./pointCloudData 为路径
     * PointCloud 为过滤器文件名
     */
    QString fileName = QFileDialog::getOpenFileName(this, "Open PointCloud File", "../../pointCloudData",
                                                    "PointCloud (*.pcd *.txt *.las)");
    if (fileName.isEmpty()) {
        return;
    }

    originalPCDFileName = QFileInfo(fileName).fileName();

    QString octreeFileName(fileName);
    if (QFileInfo(fileName).suffix() == "pcd") {
        // 创建.dataOctree隐藏文件夹用来保存八叉树索引文件
        QString octreeDir = QFileInfo(fileName).absolutePath() + "/.dataOctree";
        QDir fdir(octreeDir);
        if (!fdir.exists()) {
            fdir.mkpath(octreeDir);
        }

        QString octreeFileDir =
                QFileInfo(fileName).absolutePath() + "/.dataOctree/" + QFileInfo(fileName).fileName() + ".tree";
        QDir dir(octreeFileDir);
        qDebug() << "octreeFileDir: " << octreeFileDir;

        // 隐藏文件夹中没有该文件的索引文件，则产生一个索引文件
        if (!dir.exists()) {
            progressDialog->setWindowTitle("HDMaps");
            progressDialog->setWindowModality(Qt::ApplicationModal);
            progressDialog->setMinimumDuration(0);
            generateOctreeThread.start();
            emit startGenerateOctreeDataSignal(fileName, hasRGB);
            progressDialog->exec();
        }

        // 设置文件过滤器
        dir.setNameFilters({"*.pcd"});

        // 迭代所有的 pcd 文件
        QFileInfoList list = dir.entryInfoList();
        for (const QFileInfo &fileInfo : list) {
            octreeFileName = fileInfo.absoluteFilePath();
            qDebug() << "octreeFileName: " << octreeFileName;
            break;
        }
    }

    openFileInfo.setFile(octreeFileName);
    // 读取源文件 并把该文件加入到场景树中
    osgWidget->readPCDataFromFile(octreeFileName, hasIntensity, hasRGB, originalPCDFileName);
    // 加载容器的内容
    osgWidget->loadVectorMap();
    // 初始化操作器 例如 鼠标对场景的操作
    osgWidget->initTerrainManipulator();
}

void MainWindow::openFileInfoSlot(QString &fileExtension, bool hasIntensity, bool hasRGB) {
    this->fileExtension = fileExtension;
    this->hasIntensity = hasIntensity;
    this->hasRGB = hasRGB;
}

void MainWindow::saveFile() {
    QDir dir;
    // 调试用上面一个，因为是外围build编译，部署发布用下面，数据和可执行文件在同一层
    // dir.cd("../../");

    // 文件未打开
    if (openFileInfo.fileName().isEmpty()) {
        return;
    }

    QString fileName;

    if (openFileInfo.suffix() == "pcd") {
        fileName = originalPCDFileName;
    } else {
        fileName = openFileInfo.fileName();
    }

    QString dirName = "data/" + fileName + "/";
    if (!dir.exists(dirName)) {
        std::cout << "data directory doesn't exists, try to create it " << std::endl;
        dir.mkpath(dirName);
    }

    dir.cd(dirName);
    std::cout << "save to: " << dir.absolutePath().toStdString() << std::endl;
    osgWidget->saveVectorMap(dir.absolutePath().toStdString());
    osgWidget->transVectorMapToJson(dir.absolutePath().toStdString());
    osgWidget->transVectorMapToCSV(dir.absolutePath().toStdString());
    //osgWidget->transAllPointsToJSON(dir.absolutePath().toStdString());
}

void MainWindow::saveAsFile() {
    QDir dir;
    // 调试用上面一个，因为是外围build编译，部署发布用下面，数据和可执行文件在同一层
    // dir.cd("../../");

    // 文件未打开
    if (openFileInfo.fileName().isEmpty()) {
        return;
    }

    QString fileName;

    if (openFileInfo.suffix() == "pcd") {
        fileName = originalPCDFileName;
    } else {
        fileName = openFileInfo.fileName();
    }

    QString dirName = "data/" + fileName + "/";
    if (!dir.exists(dirName)) {
        std::cout << "data directory doesn't exists, try to create it " << std::endl;
        dir.mkpath(dirName);
    }

    dir.cd(dirName);
    std::cout << "save MetaData to: " << dir.absolutePath().toStdString() << std::endl;
    osgWidget->saveVectorMap(dir.absolutePath().toStdString());

    QString saveDir =
            QFileDialog::getSaveFileName(this, "Save Json and CSV Data", "./data/dummy.json", "Data (*.json *.CSV)");
    if (saveDir.isEmpty()) {
        QMessageBox::warning(nullptr, "Save File", "Save cancel", QMessageBox::Yes);
        return;
    }
    dir.cd(QFileInfo(saveDir).absolutePath());
    std::cout << "save customData to: " << dir.absolutePath().toStdString() << std::endl;
    osgWidget->transVectorMapToJson(dir.absolutePath().toStdString());
    osgWidget->transVectorMapToCSV(dir.absolutePath().toStdString());
    //osgWidget->transAllPointsToJSON(dir.absolutePath().toStdString());
}

void MainWindow::closeCurrentFile() {
    if (QMessageBox::warning(nullptr,
                             "Close Current File",
                             "Save file or not?",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes) == QMessageBox::Yes) {
        saveFile();
    }
    // 文件关闭时关闭程序不会提醒保存
    openFileInfo.setFile(nullptr);
    osgWidget->reset();
}

void MainWindow::saveJsonFile() {
    QDir dir;
    dir.cd("../../");

    // 文件未打开
    if (openFileInfo.fileName().isEmpty()) {
        return;
    }

    QString dirName = "data/" + openFileInfo.fileName() + "/";
    if (!dir.exists(dirName)) {
        std::cout << "data directory doesn't exists, try to create it " << std::endl;
        dir.mkpath(dirName);
    }

    dir.cd(dirName);
    std::cout << "save to: " << dir.absolutePath().toStdString() << std::endl;
    osgWidget->transVectorMapToJson(dir.absolutePath().toStdString());
}

void MainWindow::quitApp() {
    QApplication::quit();
}

void MainWindow::drawLine(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != drawLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawLineAction;
    }

    osgWidget->activeLineEditor(isActive);
}

void MainWindow::modifyLine(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != modifyLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = modifyLineAction;
    }

    osgWidget->activeLineModification(isActive);
}

void MainWindow::deleteLine(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deleteLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = deleteLineAction;
    }

    osgWidget->activeLineDeletion(isActive);
}

void MainWindow::drawTraceLine(bool isActive) {
    if (isActive) {

        /*
         *
         */

        if (clickedEditorAction && clickedEditorAction != drawTraceLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawTraceLineAction;
    }

    osgWidget->activeTraceLineEditor(isActive);
}

void MainWindow::modifyTraceLine(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != modifyTraceLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = modifyTraceLineAction;
    }

    osgWidget->activeTraceLineModification(isActive);
}

void MainWindow::deleteTraceLine(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deleteTraceLineAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = deleteTraceLineAction;
    }

    osgWidget->activeTraceLineDeletion(isActive);
}

void MainWindow::drawPavement(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != drawPavementAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawPavementAction;
    }

    osgWidget->activePavementEditor(isActive);
}

void MainWindow::deletePavement(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deletePavementAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = deletePavementAction;
    }

    osgWidget->activePavementDeletion(isActive);
}

void MainWindow::drawDrivingArrow(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != drawDrivingArrowAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawDrivingArrowAction;
    }

    osgWidget->activeDrivingArrowEditor(isActive);
}

void MainWindow::modifyDrivingArrow(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != modifyDrivingArrowAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = modifyDrivingArrowAction;
    }

    osgWidget->activeDrivingArrowModification(isActive);
}

void MainWindow::deleteDrivingArrow(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deleteDrivingArrowAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = deleteDrivingArrowAction;
    }

    osgWidget->activeDrivingArrowDeletion(isActive);
}

void MainWindow::drawTrafficLights(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != drawTrafficLightsAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawTrafficLightsAction;
    }

    osgWidget->activeTrafficLightsEditor(isActive);
}

void MainWindow::deleteTrafficLights(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deleteTrafficLightsAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = deleteTrafficLightsAction;
    }

    osgWidget->activeTrafficLightsDeletion(isActive);
}

void MainWindow::drawRoadLines(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != drawRoadLinesAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesModification(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = drawRoadLinesAction;
    }

    osgWidget->activeRoadLinesEditor(isActive);
}

void MainWindow::modifyRoadLines(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != modifyRoadLinesAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesDeletion(false);
        }
        clickedEditorAction = modifyRoadLinesAction;
    }

    osgWidget->activeRoadLinesModification(isActive);
}

void MainWindow::deleteRoadLines(bool isActive) {
    if (isActive) {
        if (clickedEditorAction && clickedEditorAction != deleteRoadLinesAction) {
            clickedEditorAction->setChecked(false);

            osgWidget->activeLineEditor(false);
            osgWidget->activeLineModification(false);
            osgWidget->activeLineDeletion(false);

            osgWidget->activeTraceLineEditor(false);
            osgWidget->activeTraceLineModification(false);
            osgWidget->activeTraceLineDeletion(false);

            osgWidget->activePavementEditor(false);
            osgWidget->activePavementDeletion(false);

            osgWidget->activeDrivingArrowEditor(false);
            osgWidget->activeDrivingArrowModification(false);
            osgWidget->activeDrivingArrowDeletion(false);

            osgWidget->activeTrafficLightsEditor(false);
            osgWidget->activeTrafficLightsDeletion(false);

            osgWidget->activeRoadLinesEditor(false);
            osgWidget->activeRoadLinesModification(false);
        }
        clickedEditorAction = deleteRoadLinesAction;
    }

    osgWidget->activeRoadLinesDeletion(isActive);
}

void MainWindow::colorByZ(bool isActive) {
    if (isActive) {
        if (clickedColorAction && clickedColorAction != colorByZAction) {
            clickedColorAction->setChecked(false);
            osgWidget->activeColorByIntensity(false);
            osgWidget->activeColorByTexture(false);
        }
        clickedColorAction = colorByZAction;
    }

    osgWidget->activeColorByZ(isActive);
}

void MainWindow::colorByIntensity(bool isActive) {
    if (isActive) {
        if (clickedColorAction && clickedColorAction != colorByIntensityAction) {
            clickedColorAction->setChecked(false);
            osgWidget->activeColorByZ(false);
            osgWidget->activeColorByTexture(false);
        }
        clickedColorAction = colorByIntensityAction;
    }

    osgWidget->activeColorByIntensity(isActive);
}

void MainWindow::colorByTexture(bool isActive) {
    if (isActive) {
        if (clickedColorAction && clickedColorAction != colorByTextureAction) {
            clickedColorAction->setChecked(false);
            osgWidget->activeColorByZ(false);
            osgWidget->activeColorByIntensity(false);
        }
        clickedColorAction = colorByTextureAction;
    }

    osgWidget->activeColorByTexture(isActive);
}

void MainWindow::help() {

}

void MainWindow::about() {
    QMessageBox::about(this, "About HDMap", "Version: 1.0");
}