#include "widget.h"
#include <QApplication>
#include <QSpinBox>
#include <QSlider>
#include <QHBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Enter your age");
    //只能输入数字的输入框,并且带有上下箭头的步进按钮。
    QSpinBox *spinBox=new QSpinBox (&w);
    //带有滑块的滑竿
    QSlider *slider= new QSlider (Qt::Horizontal,&w);
    //设置其范围
    spinBox->setRange(0,100);
    slider->setRange(0,100);

    //slider->spinBox
    /*
     *  在这里 我们设置的信号槽关联关系是 slider->spinBox
        为什么不使用 spinBox->slider 的关联方式呢？
        原因在于 spinBox 的 valueChanged() 函数被重载了为：
        1.void valueChanged(int)
        2.void valueChanged(const QString &)
        当我们用 spinBox 主动去连接 slider 系统不知道是哪一个 valueChanged()函数,所以会报错。

        为了解决这种冲突,并且实现双向关联：
        可设置一个函数指针 void (QSpinBox:: *spinBoxSignal)(int) = &QSpinBox::valueChanged;
        让这个函数指针指向 1.void valueChanged(int) 用 spinBoxSignal 去关联 slider 就不会报错了。
        关联语句： QObject::connect(spinBox, spinBoxSignal, slider, &QSlider::setValue);
     */
    QObject::connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
    void (QSpinBox:: *spinBoxSignal)(int) = &QSpinBox::valueChanged;
    QObject::connect(spinBox, spinBoxSignal, slider, &QSlider::setValue);
    spinBox->setValue(25);

    //布局管理器聪明地做出了正确的行为：保持 QSpinBox 宽度不变，自动拉伸 QSlider 的宽度。
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(spinBox);
    layout->addWidget(slider);
    w.setLayout(layout);

    w.show();

    return a.exec();
}
