#include "mainwindow.h"
#include "ui_mainwindow.h"

//
// Created by zhihui on 7/4/19.
//
#include <QFile>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    setWindowTitle("XML Reader");

    treeWidget = new QTreeWidget(this);

    QStringList headers;

    headers<<"Items"<<"Pages";
    treeWidget->setHeaderLabels(headers);

    setCentralWidget(treeWidget);


}

MainWindow::~MainWindow()
{

}

//bool MainWindow::readFile(const QString &fileName)
//{
//    // 定义一个QFile 类的对象
//    QFile file(fileName);

//    if(!file.open(QFile::ReadOnly | QFile::Text))
//    {
//        QMessageBox::critical(this, tr("Error"),
//                             tr("Cannot read file %1").arg(fileName));
//        return false;
//    }
//    /*
//     * 我们使用 QFile 打开文件，将其设置为 QXmlStreamReader 的设备
//     * 然后我们可以利用 QXmlStreamReader 从这个设备（QFile）中读取内容去分析了。
//     */
//    reader.setDevice(&file);

//    /*
//     * while 循环：只要没有读到文件尾，我们一直进行循环判断。
//     * 首先判断是不是 StartElement？ 如果是我们再判断 它的标签：bookindex。 如果不是 StartElement（第一次进入循环的时候，由于没有事先调用 readNext()，所以会进入这个分支），则调用readNext()。
//     *  如果是 根标签：bookindex 我们就去读它的标签元素 不是则说明标签不对，发起一个错误。
//     */
//    while(!reader.atEnd())
//    {
//        if(reader.isStartElement())
//        {
//            if(reader.name()=="bookindex ")
//            {
//                readBookindexElement();
//            } else
//            {
//                reader.raiseError("Not a valid book file");
//            }
//        }
//        else
//        {
//            reader.readNext();
//        }
//    }
//    /*
//     * 注：
//     * 为什么这里要用 while 循环，XML 文档不是只有一个根标签吗？
//     * 直接调用一次 readNext() 函数不就好了？
//     * 这是因为，XML 文档在根标签之前还有别的内容，比如声明，比如 DTD，我们不能确定第一个 readNext() 之后就是根标签。
//     * 正如我们提供的这个 XML 文档，首先是 声明，其次才是根标签。
//     * 如果你说，第二个不就是根标签吗？但是 XML 文档还允许嵌入 DTD，还可以写注释，这就不确定数目了，所以为了通用起见，我们必须用 while 循环判断。
//     */


//    /*
//     * 处理完 我们就可以关闭文件了。
//     */
//    file.close();

//    /*
//     * 如果有错误 就显示错误。
//     */
//    if(reader.hasError())
//    {
//        QMessageBox::critical(this, tr("Error"),
//                                tr("Failed to parse file %1").arg(fileName));
//        return false;
//    }
//    else if (file.error() != QFile::NoError)
//    {
//        QMessageBox::critical(this, tr("Error"),
//                                tr("Cannot to read file %1").arg(fileName));
//        return false;
//    }
//    else
//    {
//        return true;
//    }
//}

bool MainWindow::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Cannot read file %1").arg(fileName));
        return false;
    }
    reader.setDevice(&file);
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name() == "bookindex") {
                readBookindexElement();
            } else {
                reader.raiseError(tr("Not a valid book file"));
            }
        } else {
            reader.readNext();
        }
    }
    file.close();
    if (reader.hasError()) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Failed to parse file %1").arg(fileName));
        return false;
    } else if (file.error() != QFile::NoError) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Cannot read file %1").arg(fileName));
        return false;
    }
    return true;
}

/*
 * 注意第一行我们加了一个断言。意思是，如果在进入函数的时候，reader 不是 StartElement 状态，或者说标签不是 bookindex，就认为出错。
 * 然后继续调用 readNext()，获取下面的数据。后面还是 while 循环。如果是 EndElement，退出，
 * 如果又是 StartElement，说明是 entry 标签（注意我们的 XML 结构，bookindex 的子元素就是 entry），
 * 那么开始处理 entry，否则跳过。
 */
void MainWindow::readBookindexElement() {
    Q_ASSERT(reader.isStartElement() && reader.name() == "bookindex");
    reader.readNext();

    while (!reader.atEnd()){

        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "entry") {
                readEntryElement(treeWidget->invisibleRootItem());
            } else {
                skipUnknowElement();
            }
        } else {
            reader.readNext();
        }
    }
}

/*
 * 这个函数接受一个 QTreeWidgetItem 指针，作为根节点。
 * 这个节点被当做这个 entry 标签在 QTreeWidget 中的根节点。我们设置其名字是 entry 的 term 属性的值。
 * 然后继续读取下一个数据。同样使用 while 循环，如果是 EndElement 就继续读取；如果是 StartElement，则按需调用 readEntryElement() 或者 readPageElement()。
 * 由于 entry 标签是可以嵌套的，所以这里有一个递归调用。如果既不是 entry 也不是 page，则跳过位置标签。
 */
void MainWindow::readEntryElement(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, reader.attributes().value("term").toString());

    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if(reader.isStartElement())
        {
            if(reader.name()=="entry")
            {
                readEntryElement(item);
            }
            else if(reader.name()=="page")
            {
                readPageElement(item);
            } else
            {
                skipUnknowElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

/*
 * 由于 page 是叶子节点，没有子节点，所以不需要使用 while 循环读取。
 * 我们只是遍历了 entry 下所有的 page 标签，将其拼接成合适的字符串。
 */
void MainWindow::readPageElement(QTreeWidgetItem *parent)
{
    QString page=reader.readElementText();

    if(reader.isEndElement())
    {
        reader.readNext();
    }

    QString allpages=parent->text(1);
    if(!allpages.isEmpty())
    {
        allpages+=",";
    }

    allpages+=page;
    parent->setText(1, allpages);
}

/*
 * 我们没办法确定到底要跳过多少位置标签，所以还是得用 while 循环读取，注意位置标签中所有子标签都是未知的，
 * 因此只要是 StartElement，都直接跳过。
 */
void MainWindow::skipUnknowElement()
{
    reader.readNext();

    while(!reader.atEnd())
    {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if(reader.isStartElement())
        {
            skipUnknowElement();
        }
        else
        {
            reader.readNext();
        }
    }
}




