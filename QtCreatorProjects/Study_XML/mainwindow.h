#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QXmlStreamReader>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool readFile(const QString &fileName);

private:
    Ui::MainWindow *ui;


private:
    // 根标签
    void readBookindexElement();

    void readEntryElement(QTreeWidgetItem *parent);

    void readPageElement(QTreeWidgetItem *parent);

    void skipUnknowElement();

    QTreeWidget *treeWidget;
    QXmlStreamReader reader;
};

#endif // MAINWINDOW_H
