#ifndef DOWNLOADCONTROL_H
#define DOWNLOADCONTROL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QEventLoop>
#include <QVector>
#include "downloadthread.h"

//管理下载线程
class DownloadControl : public QObject
{
    Q_OBJECT
public:
    enum State{Waiting,Downloading,Pause,Stop,Finished};

    explicit DownloadControl(QObject *parent = 0);

    QString errorString();

    //获取下载链接大小
    qint64 getFileSize(QString url, int tryTimes = 3);

    //开始下载
    bool downloadFile(QString _url, QString _saveFile, int _count);

    //通过配置文件继续下载
    bool downloadFile(QString iniFile);

    //获取配置文件中设定的线程数
    int getSettingCount(QString url);

    State getState(){return state;}
    QString getUrl(){return url;}
    QString getSaveFile(){return saveFile;}
    int getCount(){return count;}
    int getRunningCount(){return runningCount;}
signals:
    //完成信号，0表示全部完成
    void finished(int index);

    //进度改变信号，0表示中进度
    void progressChange(int index,qint64 startPoint,qint64 endPoint,qint64 readySize);

    //错误信号
    void error(int index, QString errorString);
public slots:
    //暂停
    void pause();

    //继续
    void restart();

    //停止，该函数会在下载文件目录自动生成一个setting文件，可以通过该文件继续下载
    void stop();

private slots:
    void finishedSlot(int index);

    void progressChangeSlot(int index,qint64 startPoint,qint64 endPoint,qint64 readySize);

    void errorSlot(int index, QString errorString);
private:
    void downloadingFinish();

    QString errorInfo;
    State state;
    QVector<DownloadThread*> threads;
    QString url;
    QString saveFile;
    int count,runningCount;
    qint64 readySize,totalSize;
    QFile *file;
};

#endif // DOWNLOADCONTROL_H
