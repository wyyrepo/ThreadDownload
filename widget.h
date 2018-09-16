#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QVector>
#include <QTimer>

#include "downloadcontrol.h"
#include "config.h"
#include "downloadwidgets.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(int type,QString url,QString saveFile = QString(), QWidget *parent = 0);
    ~Widget();

private slots:
    void finishedSlot(int index);

    void progressChangeSlot(int index,qint64 startPoint,qint64 endPoint,qint64 readySize);

    void errorSlot(int index, QString errorString);

    void start();

    void pause();

    void restart();

    void stop();

    void speedSlot();
private:
    DownloadControl *control;
    int type;
    QString url,saveFile;
    QLabel *fileName,*errorLabel;
    QSpinBox *countSpinBox,*runningCountSpinBox;
    QPushButton *startBtn,*pauseBtn,*restartBtn,*stopBtn;
    QListWidget *listWidget;
    QVector<DownloadWidgets*> widgets;
    QTimer *timer;

    void initWidget();
};

#endif // WIDGET_H
