#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*
     * file->addAction(openAction);
     * toolBar->addAction(openAction);
     * 在这里我们可以看到 为窗口添加 状态栏和工具栏时，都只传入了一个参数 openAction
       那是因为在 Qmenu 和 QToolBar 类中，已经重载好了相应的函数。
       在这里我们还可以把 openAction 当成一个容器 它里面载入了不同的数据类型
     */
    ui->setupUi(this);

    setWindowTitle(tr("Main Window"));
    //设置菜单栏图片的链接
    openAction=new QAction(QIcon(":/images/doc-open"),tr("&Open..."),this);
    //home/zhihui/QtCreatorProjects/samp_Mainwindow/images/test.png
    //设置快捷键
    openAction->setShortcuts(QKeySequence::Open);

    //设置状态栏的提示信息
    openAction->setStatusTip(tr("Open an existing file"));

    //信号槽：单击图片->启动open()函数
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    //添加一个菜单栏File 并为它添加 快捷键
    QMenu *file = menuBar()->addMenu(tr("&File"));  
    file->addAction(openAction);

    //添加一个工具栏栏File 并为它添加 快捷键
    QToolBar *toolBar = addToolBar(tr("&File"));   
    toolBar->addAction(openAction);

    //添加第二个工具栏
    QAction *openAction2= new QAction(QIcon(":/images/fr/doc-open"),tr("&Cool Boy"),this);
    connect(openAction2, &QAction::triggered, this, &MainWindow::open);
    QMenu *file2 = menuBar()->addMenu(tr("&File"));
    file2->addAction(openAction2);
    QToolBar *toolBar2 = addToolBar(tr("Tool Bar 2"));
    toolBar2->addAction(openAction2);



    //为窗口设置一个工具栏
    statusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
   //消息框
   QMessageBox::information(this, tr("Information"), tr("Open"));
}
