/********************************************************************************
** Form generated from reading UI file 'qwdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QWDIALOG_H
#define UI_QWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_QWDialog
{
public:
    QCheckBox *checkBox;
    QCheckBox *checkBox_3;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QCheckBox *cbItalic;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *QWDialog)
    {
        if (QWDialog->objectName().isEmpty())
            QWDialog->setObjectName(QString::fromUtf8("QWDialog"));
        QWDialog->resize(338, 314);
        QFont font;
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        QWDialog->setFont(font);
        checkBox = new QCheckBox(QWDialog);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(156, 11, 99, 23));
        QFont font1;
        font1.setUnderline(false);
        checkBox->setFont(font1);
        checkBox_3 = new QCheckBox(QWDialog);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setGeometry(QRect(261, 11, 51, 23));
        QFont font2;
        font2.setItalic(false);
        checkBox_3->setFont(font2);
        radioButton = new QRadioButton(QWDialog);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(191, 51, 56, 23));
        radioButton_2 = new QRadioButton(QWDialog);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(81, 51, 47, 23));
        radioButton_3 = new QRadioButton(QWDialog);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));
        radioButton_3->setGeometry(QRect(134, 51, 51, 23));
        pushButton = new QPushButton(QWDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(61, 241, 80, 25));
        pushButton_2 = new QPushButton(QWDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(147, 241, 80, 25));
        pushButton_3 = new QPushButton(QWDialog);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(233, 241, 80, 25));
        cbItalic = new QCheckBox(QWDialog);
        cbItalic->setObjectName(QString::fromUtf8("cbItalic"));
        cbItalic->setGeometry(QRect(60, 10, 82, 23));
        plainTextEdit = new QPlainTextEdit(QWDialog);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(70, 80, 231, 151));
        QWidget::setTabOrder(checkBox, checkBox_3);
        QWidget::setTabOrder(checkBox_3, radioButton_2);
        QWidget::setTabOrder(radioButton_2, radioButton_3);
        QWidget::setTabOrder(radioButton_3, radioButton);
        QWidget::setTabOrder(radioButton, pushButton);
        QWidget::setTabOrder(pushButton, pushButton_2);
        QWidget::setTabOrder(pushButton_2, pushButton_3);

        retranslateUi(QWDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), QWDialog, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), QWDialog, SLOT(reject()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), QWDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(QWDialog);
    } // setupUi

    void retranslateUi(QDialog *QWDialog)
    {
        QWDialog->setWindowTitle(QApplication::translate("QWDialog", "QWDialog", nullptr));
        checkBox->setText(QApplication::translate("QWDialog", "Underline(&M)", nullptr));
        checkBox_3->setText(QApplication::translate("QWDialog", "Bold", nullptr));
        radioButton->setText(QApplication::translate("QWDialog", "Black", nullptr));
        radioButton_2->setText(QApplication::translate("QWDialog", "Red", nullptr));
        radioButton_3->setText(QApplication::translate("QWDialog", "Bule", nullptr));
        pushButton->setText(QApplication::translate("QWDialog", "submit", nullptr));
        pushButton_2->setText(QApplication::translate("QWDialog", "cancle", nullptr));
        pushButton_3->setText(QApplication::translate("QWDialog", "close", nullptr));
        cbItalic->setText(QApplication::translate("QWDialog", "Italic", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QWDialog: public Ui_QWDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QWDIALOG_H
