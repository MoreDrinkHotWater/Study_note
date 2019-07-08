#include "dialog.h"
#include "ui_dialog.h"
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_2_clicked()
{
    QString curPath=QDir::currentPath();
    QString d1gTitle="选择一个文件";
    QString filter="文本文件(*.txt);;图片文件(*.jpg *.gif *.png);;所有文件(*.*)";
    QString aFileName=QFileDialog::getOpenFileName(this,d1gTitle,curPath,filter);

    if(!aFileName.isEmpty())
    {
        ui->plainTextEdit->appendPlainText(aFileName);
    }
}

void Dialog::on_pushButton_3_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString d1gTitle="保存文件";
    QString filter="文本文件(*.txt);;h文件(*.h);;C++文件(.cpp);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,d1gTitle,curPath,filter);
    if (!aFileName.isEmpty())
        ui->plainTextEdit->appendPlainText(aFileName);
}

void Dialog::on_pushButton_clicked()
{
    QString dlgTitle="输入整数对话框";
    QString txtLabel="设置字体大小";
    int defaultValue=ui->plainTextEdit->font().pointSize(); //现有字体大小
    int minValue=6, maxValue=50,stepValue=1; //范围，步长
    bool ok=false;
    int inputValue = QInputDialog::getInt(this, dlgTitle,txtLabel,
                               defaultValue, minValue,maxValue,stepValue,&ok);
    if (ok) //是否确认输入
    {
        QFont   font=ui->plainTextEdit->font();
        font.setPointSize(inputValue);
        ui->plainTextEdit->setFont(font);
    }
}

void Dialog::on_pushButton_4_clicked()
{
    QString dlgTitle="warning 消息框";
    QString strInfo="文件内容已经被修改";
    QMessageBox::warning(this, dlgTitle, strInfo);
}

void Dialog::on_pushButton_6_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="选择多个文件"; //对话框标题
    QString filter="文本文件(*.txt);;图片文件(*.jpg *.gif *.png);;所有文件(*.*)"; //文件过滤器
    QStringList fileList=QFileDialog::getOpenFileNames(this,dlgTitle,curPath,filter);
    for (int i=0; i<fileList.count();i++)
        ui->plainTextEdit->appendPlainText(fileList.at(i));
}
