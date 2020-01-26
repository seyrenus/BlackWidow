#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"Main::MainWIndow()";
    //加载配置文件
    QString configPath = QDir::currentPath()+QDir::separator()+"config.json";
    jsonp = new JsonParser(configPath);
    QString dbPath = jsonp->getValue("db");
    qDebug()<<"db path in config is: "<<dbPath;

    dbp = new DBProcessor("main",dbPath);
}

MainWindow::~MainWindow()
{
    delete dbp;
    delete jsonp;
    delete ui;
}

void MainWindow::on_actionReset_triggered()
{
    bool isOk = dbp->reset();
    if(isOk){
        QMessageBox::information(this,tr("Tips"),tr("Reset successfully"),QMessageBox::Ok);
    }
}
