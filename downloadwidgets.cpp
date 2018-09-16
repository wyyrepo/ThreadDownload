#include "downloadwidgets.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

DownloadWidgets::DownloadWidgets(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    progress = new QProgressBar(this);
    mainLayout->addWidget(progress);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel("已下载："));
    readyLabel = new QLabel("0 B");
    layout->addWidget(readyLabel);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));

    layout->addWidget(new QLabel("总大小："));
    totalLabel = new QLabel("0 B");
    layout->addWidget(totalLabel);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));

    layout->addWidget(new QLabel("速度："));
    speedLabel = new QLabel("0 B/s");
    layout->addWidget(speedLabel);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));

    mainLayout->addLayout(layout);
    mainLayout->setContentsMargins(0,0,10,10);//非常重要，否则可能无法再QListWidget中显示

    setLayout(mainLayout);

    readySize = readySize2 = totalSize = 0;
    progress->setMaximum(0);
}

QString DownloadWidgets::friendlyDisplay(double size)
{
    static const double eps = 1e-3;
    QString sub = "B";

    if(size > 1024 + eps)
    {
        size /= 1024.0;
        sub = "KB";
        if(size > 1024 + eps)
        {
            size /= 1024.0;
            sub = "MB";
            if(size > 1024 + eps)
            {
                size /= 1024.0;
                sub = "G";
            }
        }
    }
    return QString::number(size,'f',2) + " " + sub;
}

void DownloadWidgets::updateReady(qint64 startPoint, qint64 endPoint, qint64 readySize)
{
    readySize2 = readySize;
    totalSize = endPoint - startPoint;
    qint64 div = 1;
    while(int(totalSize/div) > 1e8)div *= 10;//进度条只能设置int范围内的进度，超出这个范围可能会出错
    progress->setMinimum(0);
    progress->setMaximum(totalSize/div);
    progress->setValue(readySize2/div);
    if(totalSize <= readySize2)progress->setValue(totalSize/div);
    readyLabel->setText(friendlyDisplay(readySize2));
    totalLabel->setText(friendlyDisplay(totalSize));
}

void DownloadWidgets::updateSpeed()
{
    qint64 derta = readySize2 - readySize;
    readySize = readySize2;
    speedLabel->setText(friendlyDisplay(derta) + "/s");
}
