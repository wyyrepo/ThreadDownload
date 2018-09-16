#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include "config.h"
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget *w;

    int btn = QMessageBox::information(0,title,"请选择：","新建下载","继续下载","取消");
    qDebug()<<btn;
    if(btn == 0)
    {
        QString url = QInputDialog::getText(0,title,"请输入下载链接");
        if(url.isEmpty())return 0;
        QString file = QFileDialog::getSaveFileName(0,title);
        if(file.isEmpty())return 0;
        file = QFileInfo(file).absoluteFilePath();
        w = new Widget(0,url,file);
    }else if(btn == 1){
        QString url = QFileDialog::getOpenFileName(0,title,"./","setting file (*.setting)");
        if(url.isEmpty())return 0;
        w = new Widget(1,url);
    }else{
        return 0;
    }
    w->show();

    return a.exec();
}
