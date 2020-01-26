#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->QTextQuestion->setAcceptDrops(false);
    this->setAcceptDrops(true);

    //初始化变量
    isStart = false;

    id=-1;

    websiteNameList.clear();

    model = new QStandardItemModel();

    ojspider = new Spider();
    connect(ojspider,&Spider::countChanged,this,&MainWindow::countChanged);
    connect(ojspider,&Spider::displayMsg,this,&MainWindow::displayMsg);
    connect(ojspider,&Spider::updateModel,this,&MainWindow::updateModel);
    connect(this,&MainWindow::platformName,ojspider,&Spider::getPlatformName);


    queryData = new Database("task","blackwidow.db");

    idMax = queryData->getMaxId("data");//获取数据库中最大的ID

    model = new QStandardItemModel();

    lighter = new Highlighter(ui->QTextAnswer->document());

    //初始化功能函数
    loadJsonFile();

    //初始化界面
    ui->QLineId->setText(QString::number(queryData->getMaxId("data")));
    ui->QCBLevel->addItems(queryData->getTableList("level"));
    ui->QCBLanguage->addItems(queryData->getTableList("language"));
    ui->QCBPlatform->addItems(websiteNameList);
    ui->QCBType->addItems(queryData->getTableList("type"));

    //设置表格界面
    model->setColumnCount(3);
    model->setHorizontalHeaderItem(0,new QStandardItem("name"));
    model->setHorizontalHeaderItem(1,new QStandardItem("platform"));
    model->setHorizontalHeaderItem(2,new QStandardItem("id"));
    ui->SpiderTableView->horizontalHeader()->setStretchLastSection(true);
    ui->SpiderTableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}
//加载website.json配置文件，获取数据
bool MainWindow::loadJsonFile()
{
    //添加平台名称至下拉列表
    QString jsonFilePath = QDir::currentPath() + QDir::separator() + "website.json";
    //QString jsonFilePath = QString("C:/Users/jackey/Documents/BlackWidow/src/resources/")+QString("website.json");
    qDebug()<<"Json file path is: "<<jsonFilePath;
    QFile jsonFile(jsonFilePath);
    if(!jsonFile.exists()){
        qDebug()<<"App configuration json file do not exists";
        return false;
    }
    if(!jsonFile.open(QIODevice::ReadOnly)){
        qDebug()<<"Couldn't open project configuration json file";
        return false;
    }
    QByteArray allData = jsonFile.readAll();
    jsonFile.close();
    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&json_error));
    if(json_error.error !=QJsonParseError::NoError){
        qDebug()<<"json error: "<<json_error.errorString();
        return false;
    }
    QJsonObject rootObj = jsonDoc.object();
    QStringList keys = rootObj.keys();
    ui->SpiderBoxPlatform->addItems(keys);

    websiteNameList = keys;
    qDebug()<<websiteNameList;

    return true;
}

void MainWindow::displayDetailById(int qid)
{
    id = qid;//表示当前操作ID号
    qDebug()<<"Current display id is: "<<qid;
    if(qid>idMax || qid<0){
        qDebug()<<"The question id is wrong";
        return;
    }
     qDebug()<<"Current display question id: "<<id;
     sql = QString("select * from data where id=%1").arg(id);
     queryData->query->prepare(sql);
     if(queryData->query->exec()){
         while(queryData->query->next()){
             spiderData.type = queryData->query->value("type").toString();
             spiderData.level = queryData->query->value("level").toInt();
             spiderData.question = queryData->query->value("detail").toString();
             spiderData.answer = queryData->query->value("answer").toString();
             spiderData.tip = queryData->query->value("tip").toString();
             spiderData.id = id;
             spiderData.language = queryData->query->value("language").toString();
             spiderData.name = queryData->query->value("name").toString();
             spiderData.passed = queryData->query->value("passed").toString();
             spiderData.rate = queryData->query->value("rate").toString();
             spiderData.submit = queryData->query->value("submit").toString();
             spiderData.platform = queryData->query->value("platform").toInt();
             spiderData.cata = queryData->query->value("cata").toString();
         }
     }
     else{
         qDebug()<<"Cannot query question detail data: "<<queryData->query->lastError();
     }

     if(spiderData.question.isEmpty()){
         if(isNext){
             displayDetailById(qid+1);
         }
         else{
             displayDetailById(qid-1);
         }
     }

     on_actionClear_triggered();
     ui->QCBType->setCurrentText(spiderData.type);
     int level = spiderData.level-1;
     if(level<0) level = 8;
     ui->QCBLevel->setCurrentIndex(level);
     ui->QTextQuestion->setHtml(spiderData.question);
     ui->QLineId->setText(QString::number(id));
     ui->QCBLanguage->setCurrentText(spiderData.language);
     ui->QLineName->setText(spiderData.name);
     ui->QLinePassed->setText(spiderData.passed);
     ui->QLineRate->setText(spiderData.rate);
     ui->QLineSubmit->setText(spiderData.submit);
     ui->QCBPlatform->setCurrentIndex(spiderData.platform);
     ui->QPlainTextCata->setPlainText(spiderData.cata);

     QString newTitle=QString("id: %1 level: %2").arg(id).arg(spiderData.level);
     this->setWindowTitle(newTitle);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if(event->source()==this)
        event->ignore();
    else  event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty()) return;
    foreach (QUrl url, urls) {
        QString imagePath = QString("img")+QDir::separator()+url.toString().split('/').last();
        QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(QString("<img src=\'%1\' width=200 height=200 />").arg(imagePath));
        ui->QTextQuestion->textCursor().insertFragment(fragment);
        qDebug()<<"Image path is: "<<imagePath;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
}

void MainWindow::countChanged(int count)
{
    ui->SpiderLcdNumber->display(count);
}

void MainWindow::displayMsg(QString msg)
{
    QMessageBox::information(this,tr("Tips"),msg,QMessageBox::Ok);
}

void MainWindow::updateModel(QList<QStandardItem *> rowList)
{
    model->appendRow(rowList);
    ui->SpiderTableView->setModel(model);
}

void MainWindow::threadResult()
{
    int result = queryData->resultValue;
    qDebug()<<"result value is: "<<result;
    if(result >0){
        QString tip = QString("Current question is same to id: %1").arg(result);
        QMessageBox::information(this,tr("Warning"),tip,QMessageBox::Ok);
        on_actionClear_triggered();
        return;
    }
    else{
        id = queryData->getMaxId("data");
        sql = "insert into data values(:id,:platform,:name,:question,:answer,:tip,:cata,:type,:level)";
        queryData->query->prepare(sql);
        queryData->query->bindValue(":id",++id);
        int platform = ui->QCBPlatform->currentIndex()+1;
        queryData->query->bindValue(":platform",platform);
        queryData->query->bindValue(":name",ui->QLineName->text());
        queryData->query->bindValue(":question",ui->QTextQuestion->toPlainText());
        queryData->query->bindValue(":answer",ui->QTextAnswer->toPlainText());
        queryData->query->bindValue(":tip",ui->QTextTip->toPlainText());
        queryData->query->bindValue(":cata",ui->QPlainTextCata->toPlainText());
        int type = ui->QCBType->currentIndex()+1;
        queryData->query->bindValue(":type",type);
        int level = ui->QCBLevel->currentIndex()+1;
        queryData->query->bindValue(":level",level);
        queryData->query->bindValue(":id",id);
        if(queryData->query->exec()){
            qDebug()<<"Insert one question into table";
            on_actionClear_triggered();
            ui->QLineId->setText(QString::number(id));
        }
        else{
            qDebug()<<"Cannot insert data into table task: "<<queryData->query->lastError();
        }
    }
    queryData->wait();
}

void MainWindow::on_actionAbout_triggered()
{
    QString aboutMsg =QString("<h1>BlackWidow</h1>"
                              "<h2>Author: JackeyLea</h2>"
                              "<h2>License: LGPL3</h2>"
                              "<h2>E-mail: 1768478912@qq.com</h2>"
                              "<h2>Phone: 13812991101</h2>"
                              "<h3>Please keep those message showing up</h3>");
    QMessageBox::information(this,tr("About"),aboutMsg,QMessageBox::Ok);
}

void MainWindow::on_actionQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionGitHub_triggered()
{
    qDebug()<<"Open github project url with default browser";
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/BlackWidow"));
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionSpider_triggered()
{
    qDebug()<<"Show spider widget";
    ui->stackedWidget->setCurrentIndex(0);
    this->setWindowTitle(tr("Spider"));
}

void MainWindow::on_actionQuestion_triggered()
{
    qDebug()<<"Show question widget";
    ui->stackedWidget->setCurrentIndex(1);
    this->setWindowTitle(tr("Question"));
}
//如果平台下拉框被修改，那么就重置界面
void MainWindow::on_SpiderBoxPlatform_currentIndexChanged(int index)
{
    qDebug()<<"Current index is: "<<index;
    qDebug()<<"Change platform to: "<<ui->SpiderBoxPlatform->currentText();
    if(this->isVisible()){
        //设置表格界面
        model->clear();
        model->setColumnCount(3);
        model->setHorizontalHeaderItem(0,new QStandardItem("name"));
        model->setHorizontalHeaderItem(1,new QStandardItem("platform"));
        model->setHorizontalHeaderItem(2,new QStandardItem("id"));
        ui->SpiderTableView->horizontalHeader()->setStretchLastSection(true);
        ui->SpiderTableView->setModel(model);
    }
}

void MainWindow::on_actionDone_triggered()
{
    if(isStart){
        idMax = queryData->getMaxId("data");
        sql = QString("update data set status = '1' where id= %1").arg(id);
        queryData->query->prepare(sql);
        if(queryData->query->exec()){
            QMessageBox::information(this,tr("Tip"),tr("Set this question status: done"),QMessageBox::Ok);
        }
        else {
            qDebug()<<"Cannot set this question status to done: "<<queryData->query->lastError();
        }
        sql.clear();
    }
    else{
        QMessageBox::information(this,tr("Warning"),tr("Please start first"),QMessageBox::Ok);
        return;
    }
}
//显示当前题号的上一题
void MainWindow::on_actionPrevious_triggered()
{
    if(!isStart){
        QMessageBox::information(this,tr("Tip"),tr("Please start first"),QMessageBox::Ok);
        return;
    }
    if(id<=1){
        QMessageBox::information(this,tr("Warning"),tr("Display the last question"),QMessageBox::Ok);
        displayDetailById(idMax);
    }
    else {
        displayDetailById(--id);
    }
    isNext = false;
}
//显示当前题号的下一题
void MainWindow::on_actionNext_triggered()
{
    if(!isStart){
        QMessageBox::information(this,tr("Tip"),tr("Please start first"),QMessageBox::Ok);
        return;
    }
    if(id>=idMax){
        QMessageBox::information(this,tr("Tip"),tr("We are in beginning"),QMessageBox::Ok);
        displayDetailById(1);
    }
    else {
        displayDetailById(++id);
    }

    isNext = true;
}
//开始或结束题目操作
void MainWindow::on_actionStart_End_triggered()
{
    if(!isStart){
        qDebug()<<"Start examination";
        idMax = queryData->getMaxId("data");
        qDebug()<<"id max is: "<<idMax;
        displayDetailById(1);
        isStart = true;
    }
    else {
        qDebug()<<"Examination end";
        isStart = false;
        spiderData.question.clear();
        spiderData.answer.clear();
        spiderData.tip.clear();
        on_actionClear_triggered();//清空界面
    }
}
//操作给定ID的题目
void MainWindow::on_QLineId_returnPressed()
{
    id = ui->QLineId->text().toInt();
    displayDetailById(id);
    isStart = true;
}

void MainWindow::on_actionSubmit_triggered()
{
    if(!isStart) {
        qDebug()<<"Please start questin first";
        return;
    }
    if(ui->QTextQuestion->toPlainText().isEmpty()){
        QMessageBox::information(this,tr("Warning"),tr("Missing essential text"),QMessageBox::Ok);
        return;
    }
    queryData->tableName = "task";
    queryData->columName = "question";
    queryData->var = ui->QTextQuestion->toPlainText();
    queryData->run();
    connect(queryData,SIGNAL(finished()),this,SLOT(threadResult()));
}

void MainWindow::on_actionUpdate_triggered()
{
    if(!isStart) {
        qDebug()<<"Please start questin first";
        return;
    }
    sql = "update data set platform=:platform,name=:name,question=:question,"
          "answer=:answer,tip=:tip,cata=:cata,type=:type,level=:level where id=:id";
    queryData->query->prepare(sql);
    int platform = ui->QCBPlatform->currentIndex()+1;
    queryData->query->bindValue(":platform",platform);
    queryData->query->bindValue(":name",ui->QLineName->text());
    queryData->query->bindValue(":question",ui->QTextQuestion->toPlainText());
    queryData->query->bindValue(":answer",ui->QTextAnswer->toPlainText());
    queryData->query->bindValue(":tip",ui->QTextTip->toPlainText());
    queryData->query->bindValue(":cata",ui->QPlainTextCata->toPlainText());
    int type = ui->QCBType->currentIndex()+1;
    queryData->query->bindValue(":type",type);
    int level = ui->QCBLevel->currentIndex()+1;
    queryData->query->bindValue(":level",level);
    queryData->query->bindValue(":id",id);
    if(queryData->query->exec()){
        qDebug()<<"Update data successfully";
        on_actionClear_triggered();
        ui->stackedWidget->setCurrentIndex(1);
    }
    else{
        qDebug()<<"Cannot update data into database: "<<queryData->query->lastError();
    }
}

void MainWindow::on_actionClear_triggered()
{
    switch(ui->stackedWidget->currentIndex()){
    case 0:
        //设置表格界面
        model->clear();
        model->setColumnCount(3);
        model->setHorizontalHeaderItem(0,new QStandardItem("name"));
        model->setHorizontalHeaderItem(1,new QStandardItem("platform"));
        model->setHorizontalHeaderItem(2,new QStandardItem("id"));
        ui->SpiderTableView->horizontalHeader()->setStretchLastSection(true);
        ui->SpiderTableView->setModel(model);
        break;
    case 1:
        ui->QCBLanguage->setCurrentIndex(0);
        ui->QCBLevel->setCurrentIndex(0);
        ui->QCBPlatform->setCurrentIndex(0);
        ui->QCBType->setCurrentIndex(0);
        ui->QLineId->setText(QString::number(queryData->getMaxId("data")));
        ui->QLineName->clear();
        ui->QLinePassed->clear();
        ui->QLineRate->clear();
        ui->QLineSubmit->clear();
        ui->QPlainTextCata->clear();
        ui->QTextAnswer->clear();
        ui->QTextQuestion->clear();
        ui->QTextTip->clear();
        break;
    default:
        break;
    }
}

void MainWindow::on_QButtonShowAnswer_clicked()
{
    if(!isStart) {
        qDebug()<<"Please start questin first";
        return;
    }
    if(spiderData.answer.isEmpty())
        ui->QTextAnswer->setPlainText(QString("There is no answer now,please complete this question by yourself"));
    else {
        ui->QTextAnswer->setPlainText(spiderData.answer);
    }
}

void MainWindow::on_QButtonShowTip_clicked()
{
    if(!isStart) {
        qDebug()<<"Please start questin first";
        return;
    }
    if(spiderData.tip.isEmpty()) ui->QTextTip->setPlainText(QString("No tip"));
    else ui->QTextTip->setPlainText(spiderData.tip);
}

void MainWindow::on_SpiderButtonStart_clicked()
{
    QString name = ui->SpiderBoxPlatform->currentText();
    emit platformName(name);
    ojspider->start();
}

void MainWindow::on_SpiderButtonStartAll_clicked()
{
}

void MainWindow::on_SpiderButtonStop_clicked()
{
    ojspider->stop();
}
