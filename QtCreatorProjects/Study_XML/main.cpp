#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication  a(argc, argv);
     MainWindow w;
     w.readFile("/home/zhihui/XML/book.xml");
     w.show();
     return a.exec();
}
