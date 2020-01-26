#ifndef BASE_HPP
#define BASE_HPP

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
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
#include <QIcon>
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
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTextBrowser>
#include <QTextCodec>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QTextStream>
#include <QThread>
#include <QTimeEdit>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QWidget>

#include "cmath"
using namespace std;

//数据库表列名结构体
extern struct data_ {
    int     id;
    int     platform;
    QString name;
    QString question;
    int     level;
    QString type;
    QString tip;
    QString answer;
    QString language;
    QString submit;
    QString passed;
    QString rate;
    QString cata;
} spiderStruct;

extern struct conf_ {
    QString url;
    int     begin;
    QString name;
    QString detail;
    QString level;
    QString type;
    QString language;
    QString submit;
    QString passed;
    QString rate;
} Conf;

#define IndexUrl "https://github.com/JackeyLea/BlackWidow"

inline QString loadTheme(QString themeName) {
    QString themeCtx;
    QFile   file(QString(":/resources/themes/%1.qss").arg(themeName));
    qDebug() << "theme file path is:" << file.fileName();
    if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QTextStream readIn(&file);
        themeCtx = readIn.readAll();
    } else {
        qDebug() << "Cannot open file: " << file.errorString();
        qDebug() << "Application will start with no theme";
    }
    file.close();

    return themeCtx;
}

inline int randomNum(int baseNum) {
    static QTime t = QTime::currentTime(); // time 1
    QTime        T = QTime::currentTime(); // time 2
    int          i = T.msecsTo(t);
    qsrand(static_cast<unsigned int>(i));
    int randNum = qrand();
    t           = T;
    return randNum % baseNum;
}

inline void sleep(int msec) {
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

inline void log(QString logger) {
    QDateTime dt       = QDateTime::currentDateTime();
    QString   datetime = dt.toString("yyyy-MM-dd hh:mm:ss");
    QFile     file("blackwidow.log");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
        QTextStream writeOut(&file);
        writeOut.setCodec("UTF-8");
        writeOut << datetime + " " + logger;
    }
    file.close();
}

#endif // BASE_HPP
