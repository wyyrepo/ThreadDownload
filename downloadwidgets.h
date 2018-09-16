#ifndef DOWNLOADWIDGETS_H
#define DOWNLOADWIDGETS_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

//单个下载线程，在QListWidget中显示
class DownloadWidgets : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadWidgets(QWidget *parent = 0);
    QProgressBar *progress;
    QLabel *readyLabel,*totalLabel,*speedLabel;
    qint64 readySize,readySize2,totalSize;

    QString friendlyDisplay(double size);
    void updateReady(qint64 startPoint,qint64 endPoint,qint64 readySize);
    void updateSpeed();
signals:

public slots:

};

#endif // DOWNLOADWIDGETS_H
