#ifndef CONFIG_H
#define CONFIG_H

/*
 * 多线程下载器，可以使用多个线程下载同一个文件，
 * 在下载某些文件的时候，速度回快些
 * 但是由于电脑本身的带宽限制，所以速度优势不怎么明显，
 * 相当于用普通的现在起下载，不过CPU占用要高很多。。。
 *
 * 这个程序还实现了断点续传的功能
 *
 * 基本原理：
 * 先通过下载链接的header来获取文件的大小
 * 然后计算出每个线程需要下载的内存段
 * 每个线程通过设置请求header来下载自己的内存段
*/

#include <QString>

static const QString title = "多线程下载器 v1.0";

#endif // CONFIG_H
