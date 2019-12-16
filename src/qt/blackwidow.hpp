#ifndef BLACKWIDOW_HPP
#define BLACKWIDOW_HPP

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QPlainTextEdit>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QtCore/qglobal.h>
#include <QTableView>
#include <QTextCodec>
#include <QTextBrowser>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QTextStream>
#include <QtMath>
#include <QThread>
#include <QTimeEdit>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QWidget>

#include "cmath"
using namespace std;
//数据库表列名结构体
extern struct data_{
    int id;
    int platform;
    QString name;
    QString question;
    int level;
    QString type;
    QString tip;
    QString answer;
    QString language;
    QString submit;
    QString passed;
    QString rate;
    QString cata;
}spiderStruct;

extern struct conf_{
    QString url;
    int begin;
    QString name;
    QString detail;
    QString level;
    QString type;
    QString language;
    QString submit;
    QString passed;
    QString rate;
}Conf;

static QString loadSkin(QString theme)
{
    QString style;
    QFile file(QString(":/resources/skin/%1.qss").arg(theme));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream readIn(&file);
        style = readIn.readAll();
    }
    file.close();
    return style;
}

static int randomNum(int baseNum)
{
    static QTime t=QTime::currentTime();//time 1
    QTime T=QTime::currentTime();//time 2
    int i=T.msecsTo(t);
    qsrand(static_cast<unsigned int>(i));
    int randNum=qrand();
    t=T;
    return randNum%baseNum;
}

static void sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

static void log(QString logger)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString datetime = dt.toString("yyyy-MM-dd hh:mm:ss");
    QFile file("blackwidow.log");
    if(file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Append)){
        QTextStream writeOut(&file);
        writeOut.setCodec("UTF-8");
        writeOut<<datetime+" "+logger;
    }
    file.close();
}


#endif // BLACKWIDOW_HPP
