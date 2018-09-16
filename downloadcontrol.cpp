#include "downloadcontrol.h"
#include <QSettings>

DownloadControl::DownloadControl(QObject *parent) :
    QObject(parent)
{
    state = Waiting;
    file = NULL;
}

QString DownloadControl::errorString()
{
    return errorInfo;
}

qint64 DownloadControl::getFileSize(QString url ,int tryTimes)
{
    qint64 size = -1;

    //尝试tryTimes次
    while(tryTimes --)
    {
        QNetworkAccessManager manager;
        QEventLoop loop;

        //发出请求，获取目标地址的头部信息
        QNetworkReply *reply = manager.head(QNetworkRequest(url));
        if(!reply)continue;

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->error() != QNetworkReply::NoError)
        {
            errorInfo = reply->errorString();
            continue;
        }
        QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
        reply->deleteLater();
        size = var.toLongLong();
        break;
    }

    return size;
}

bool DownloadControl::downloadFile(QString _url, QString _saveFile, int _count)
{
    if(state == Downloading)
    {
        errorInfo = "is downloading a file";
        emit error(0,errorInfo);
        return false;
    }
    if(file != NULL)
    {
        errorInfo = "unknow error";
        emit error(0,errorInfo);
        return false;
    }
    url = _url;
    saveFile = _saveFile;
    count = _count;

    if(count < 1)
    {
        errorInfo = "to few threads";
        emit error(0,errorInfo);
        return false;
    }
    if(count > 15)
    {
        errorInfo = "to many threads";
        emit error(0,errorInfo);
        return false;
    }

    totalSize = getFileSize(url);
    if(totalSize == -1)
    {
        return false;
    }
    readySize = 0;

    file = new QFile(saveFile,this);
    if(!file->open(QFile::WriteOnly))
    {
        errorInfo = "can not open file : \n" + file->errorString();
        file->close();
        file = NULL;
        emit error(0,errorInfo);
        return false;
    }
    file->resize(totalSize);

    threads.clear();
    for(int i = 0;i < count;i ++)
    {
        qint64 startPoint = totalSize * i / count;
        qint64 endPoint = totalSize * (i + 1) / count;
        DownloadThread *thread = new DownloadThread(this);
        connect(thread,SIGNAL(finished(int)),SLOT(finishedSlot(int)));
        connect(thread,SIGNAL(progressChange(int,qint64,qint64,qint64)),SLOT(progressChangeSlot(int,qint64,qint64,qint64)));
        connect(thread,SIGNAL(error(int,QString)),SLOT(errorSlot(int,QString)));
        thread->startDownload(i + 1,url,file,startPoint,endPoint);
        threads.append(thread);
    }
    state = Downloading;
    runningCount = count;
    return true;
}

bool DownloadControl::downloadFile(QString iniFile)
{
    if(state == Downloading)
    {
        errorInfo = "is downloading a file";
        emit error(0,errorInfo);
        return false;
    }
    if(file != NULL)
    {
        errorInfo = "unknow error";
        emit error(0,errorInfo);
        return false;
    }
    QSettings settings(iniFile,QSettings::IniFormat);

    url = settings.value("URL").toString();
    saveFile = settings.value("SAVEFILE").toString();
    count = settings.value("COUNT",-1).toInt();

    if(count < 1)
    {
        errorInfo = "wrong setting file";
        emit error(0,errorInfo);
        return false;
    }
    if(count > 15)
    {
        errorInfo = "wrong setting file";
        emit error(0,errorInfo);
        return false;
    }

    totalSize = settings.value("TOTALSIZE",-1).toLongLong();
    if(totalSize == -1)
    {
        return false;
    }
    readySize = settings.value("READYSIZE").toLongLong();

    file = new QFile(saveFile,this);
    if(!file->open(QFile::WriteOnly | QFile::Append))
    {
        errorInfo = "can not open file : \n" + file->errorString();
        file->close();
        file = NULL;
        emit error(0,errorInfo);
        return false;
    }
    file->resize(totalSize);

    QVector<qint64> infos;
    for(int i = 0;i < count;i ++)
    {
        QString index = QString::number(i + 1);
        qint64 startPoint = settings.value("STARTPOINT" + index,-1).toLongLong();
        qint64 endPoint = settings.value("ENDPOINT" + index,-1).toLongLong();
        qint64 readySize = settings.value("READYSIZE" + index,-1).toLongLong();

        if(startPoint == -1 || endPoint == -1 || readySize == -1)
        {
            errorInfo = "wrong setting file";
            emit error(0,errorInfo);
            return false;
        }
        infos.append(startPoint);
        infos.append(endPoint);
        infos.append(readySize);
    }

    threads.clear();
    for(int i = 0;i < count;i ++)
    {
        qint64 startPoint = infos.at(i*3);
        qint64 endPoint = infos.at(i*3+1);
        qint64 readySize = infos.at(i*3+2);
        DownloadThread *thread = new DownloadThread(this);
        connect(thread,SIGNAL(finished(int)),SLOT(finishedSlot(int)));
        connect(thread,SIGNAL(progressChange(int,qint64,qint64,qint64)),SLOT(progressChangeSlot(int,qint64,qint64,qint64)));
        connect(thread,SIGNAL(error(int,QString)),SLOT(errorSlot(int,QString)));
        thread->startDownload(i + 1,url,file,startPoint,endPoint,readySize);
        threads.append(thread);
    }
    state = Downloading;
    runningCount = count;
    return true;
}

void DownloadControl::downloadingFinish()
{
    file->flush();
    file->close();
    file = NULL;
    state = Finished;
    foreach(DownloadThread *thread,threads)
        thread->deleteLater();
    threads.clear();

    emit finished(0);
}

void DownloadControl::pause()
{
    if(state != Downloading)
    {
        errorInfo = "is not downloading";
        emit error(0,errorInfo);
        return;
    }
    state = Pause;
    foreach(DownloadThread *thread,threads)
        thread->stop();
}

void DownloadControl::restart()
{
    if(state != Pause)
    {
        errorInfo = "is not paused";
        emit error(0,errorInfo);
        return;
    }
    state = Downloading;
    foreach(DownloadThread *thread,threads)
        thread->restart();
}

void DownloadControl::stop()
{
    if(state == Downloading)
    {
        state = Pause;
        foreach(DownloadThread *thread,threads)
            thread->stop();
    }
    if(state != Pause)
    {
        errorInfo = "can not stop";
        emit error(0,errorInfo);
        return;
    }

    QString iniFile = file->fileName() + ".setting";
    QSettings settings(iniFile,QSettings::IniFormat);

    settings.setValue("URL",url);
    settings.setValue("SAVEFILE",saveFile);
    settings.setValue("COUNT",count);
    settings.setValue("TOTALSIZE",totalSize);
    settings.setValue("READYSIZE",readySize);
    for(int i = 0;i < count;i ++)
    {
        QString index = QString::number(i + 1);
        settings.setValue("STARTPOINT" + index,threads.at(i)->startPoint);
        settings.setValue("ENDPOINT" + index,threads.at(i)->endPoint);
        settings.setValue("READYSIZE" + index,threads.at(i)->readySize);
    }

    file->flush();
    file->close();
    file = NULL;
    state = Stop;
    foreach(DownloadThread *thread,threads)
        thread->deleteLater();
    threads.clear();
}

void DownloadControl::finishedSlot(int index)
{
    runningCount --;
    if(runningCount == 0 && state == Downloading)
        downloadingFinish();
    emit finished(index);
}

void DownloadControl::progressChangeSlot(int index, qint64 startPoint, qint64 endPoint, qint64 readySize)
{
    this->readySize = 0;
    foreach(DownloadThread *thread,threads)
        this->readySize += thread->readySize;

    emit progressChange(index,startPoint,endPoint,readySize);
    emit progressChange(0,0,totalSize,this->readySize);
}

void DownloadControl::errorSlot(int index, QString errorString)
{
    threads.at(index-1)->stop();
    emit error(index,errorString);
}

int DownloadControl::getSettingCount(QString url)
{
    QSettings setting(url,QSettings::IniFormat);
    return setting.value("COUNT",-1).toInt();
}
