//
// Created by zhihui on 3/27/19.
//

#ifndef POINTCLOUDAPPLICATION_MAINWINDOW_H
#define POINTCLOUDAPPLICATION_MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QThread>

class OSGWidget;

class QDockWidget;

class QTreeWidget;

class QAction;

class QProgressDialog;

class GenerateOctreeWorker;

class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow() override;

 private:

  void initActions();

  void initMenu();

  void initToolBar();

  void initStatusBar();

  void createDockWidget();

 Q_SIGNALS:

  void test();

  void startGenerateOctreeDataSignal(const QString &filePath, bool hasRGB);

 public Q_SLOTS:

  void doneGernerateOctreeData();

 protected:
  void closeEvent(QCloseEvent *e) override;

 private Q_SLOTS:

  void openFile();

  void openFileInfoSlot(QString &fileExtension, bool hasIntensity, bool hasRGB);

  void saveFile();

  void saveAsFile();

  void closeCurrentFile();

  // 暂时没有用到，保存的时候会写json文件
  void saveJsonFile();

  void quitApp();

  void drawLine(bool isActive);

  void modifyLine(bool isActive);

  void deleteLine(bool isActive);

  void drawTraceLine(bool isActive);

  void modifyTraceLine(bool isActive);

  void deleteTraceLine(bool isActive);

  void drawPavement(bool isActive);

  void deletePavement(bool isActive);

  void drawDrivingArrow(bool isActive);

  void modifyDrivingArrow(bool isActive);

  void deleteDrivingArrow(bool isActive);

  void drawTrafficLights(bool isActive);

  void deleteTrafficLights(bool isActive);

  void drawRoadLines(bool isActive);

  void modifyRoadLines(bool isActive);

  void deleteRoadLines(bool isActive);

  void colorByZ(bool isActive);

  void colorByIntensity(bool isActive);

  void colorByTexture(bool isActive);

  void help();

  void about();

 private:
  OSGWidget *osgWidget;

  QDockWidget *dockWidget;
  QTreeWidget *treeWidget;

  QMenuBar *pMenuBar;
  QToolBar *pToolBar;
  QStatusBar *pStatusBar;

  QAction *clickedEditorAction;
  QAction *clickedColorAction;

  QAction *openFileAction, *saveFileAction, *saveAsFileAction, *closeCurrentFileAction, *quitAppActionAction;
  QAction *drawLineAction, *modifyLineAction, *deleteLineAction;
  QAction *drawTraceLineAction, *modifyTraceLineAction, *deleteTraceLineAction;
  QAction *drawPavementAction, *deletePavementAction;
  QAction *drawDrivingArrowAction, *modifyDrivingArrowAction, *deleteDrivingArrowAction;
  QAction *drawTrafficLightsAction, *deleteTrafficLightsAction;
  QAction *drawRoadLinesAction, *modifyRoadLinesAction, *deleteRoadLinesAction;
  QAction *colorByZAction, *colorByIntensityAction, *colorByTextureAction;
  QAction *helpAction, *aboutAction;

  QFileInfo openFileInfo;

  QString originalPCDFileName;

  QString fileExtension;
  bool hasIntensity;
  bool hasRGB;

  GenerateOctreeWorker *generateOctreeWorker;
  QProgressDialog *progressDialog;

  QThread generateOctreeThread;

};

#endif //POINTCLOUDAPPLICATION_MAINWINDOW_H
