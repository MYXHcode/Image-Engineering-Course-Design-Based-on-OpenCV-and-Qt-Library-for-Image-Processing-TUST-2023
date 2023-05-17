#include "stdafx.h"
#include "DIPCD_DengLei.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{   // 应用程序类，在一个 Qt 应用程序中，该对象只有一个
    QApplication a(argc, argv);

    // 窗口对象
    DIPCD_DengLei w;

    // 显示窗口
    w.show();

    // 阻塞函数，程序进入了事件循环
    return a.exec();
}