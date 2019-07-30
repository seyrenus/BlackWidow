#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "blackwidow.hpp"
#include "database.hpp"
#include "highlighter.hpp"
#include "spider.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool loadJsonFile();
    void displayDetailById(int qid);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void platformName(QString name);

public slots:
    void countChanged(int count);
    void displayMsg(QString msg);
    void updateModel(QList<QStandardItem *> rowList);

private slots:
    void threadResult();

    void on_actionAbout_triggered();

    void on_actionQt_triggered();

    void on_actionGitHub_triggered();

    void on_actionBlack_triggered();

    void on_actionGray_triggered();

    void on_actionLight_triggered();

    void on_actionExit_triggered();

    void on_actionSpider_triggered();

    void on_actionQuestion_triggered();

    void on_SpiderBoxPlatform_currentIndexChanged(int index);

    void on_actionDone_triggered();

    void on_actionPrevious_triggered();

    void on_actionNext_triggered();

    void on_actionStart_End_triggered();

    void on_QLineId_returnPressed();

    void on_actionSubmit_triggered();

    void on_actionUpdate_triggered();

    void on_actionClear_triggered();

    void on_QButtonShowAnswer_clicked();

    void on_QButtonShowTip_clicked();

    void on_SpiderButtonStart_clicked();

    void on_SpiderButtonStartAll_clicked();

    void on_SpiderButtonStop_clicked();

private:
    Ui::MainWindow *ui;
    Spider *ojspider;

    QStringList websiteNameList;

    QStandardItemModel *model;

    Database *queryData;//数据库对象
    Highlighter *lighter;//语法高亮器
    QString sql;//数据库操作语句
    QString filePath;//用于存储文件路径
    int idMax;//最大ID值

    int id;
    bool isStart;
    bool isNext;//如果当前为空且isNext为真则获取下一题，否则获取上一题

    data_ spiderData;
};

#endif // MAINWINDOW_H
