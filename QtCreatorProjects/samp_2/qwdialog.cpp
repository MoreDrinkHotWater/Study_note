#include "qwdialog.h"
#include "ui_qwdialog.h"

QWDialog::QWDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QWDialog)
{
    ui->setupUi(this);
    connect(ui->radioButton,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
    connect(ui->radioButton_2,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
    connect(ui->radioButton_3,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
}

QWDialog::~QWDialog()
{
    delete ui;
}


void QWDialog::on_cbItalic_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setItalic(checked);
    ui->plainTextEdit->setFont(font);
}

void QWDialog::on_checkBox_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setUnderline(checked);
    ui->plainTextEdit->setFont(font);
}

void QWDialog::on_checkBox_3_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setBold(checked);
    ui->plainTextEdit->setFont(font);
}

void QWDialog::setTextFontColor()
{
        QPalette plet=ui->plainTextEdit->palette();
        if (ui->radioButton_3->isChecked())
            plet.setColor(QPalette::Text,Qt::blue);
        else if (ui->radioButton_2->isChecked())
           plet.setColor(QPalette::Text,Qt::red);
        else if (ui->radioButton->isChecked())
            plet.setColor(QPalette::Text,Qt::black);
        else
           plet.setColor(QPalette::Text,Qt::black);
        ui->plainTextEdit->setPalette(plet);
}



