#ifndef SPIDER_HPP
#define SPIDER_HPP

#include "base.hpp"
#include "dbprocessor.hpp"
#include "jsonparser.hpp"

class Spider: public QObject{
    Q_OBJECT
public:
    QString dbPath;
    conf_ Conf;
private:
    DBProcessor *dbp;
    QString cntPlatformName;
    QStandardItemModel *model;
    QString sql;
    QStringList endMark;//用于判断是否获取结束
    //网络四件套
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QEventLoop loop;

    QString UserAgent[900];//用于存储模拟头文件
    bool Stopped=false;//用于判断是否已经停止
    QList<int> idList;//当前操作的题目ID列表

    int id=-1;//数据库处理时所需要的ID

public:
    Spider():
    model(new QStandardItemModel()),
    manager(new QNetworkAccessManager()){
        qDebug()<<"Spider()::Spider()";
        //各个平台的题目结束标志
        endMark.append(QString("题目不可用!!"));///常州实验初中
        endMark.append(QString("No Such Problem"));///福州大学 杭州电子科技大学
        endMark.append(QString("403"));///杭州电子科技大学
        endMark.append(QString("题目不存在"));/*NYOJ*/
        endMark.append(QString("Can not find problem"));///POJ
        endMark.append(QString("No such problem"));///zoj
        endMark.append(QString("Problem is not Available!!"));///zjut

        loadUserAgent();//加载浏览器模拟头
    }

    ~Spider(){
        qDebug()<<"Spider()::~Spider()";
        delete model;
        delete manager;
    }

    void start()
    {
        if(dbPath.isEmpty()) {
            qDebug()<<"Didn't set the db path in spider start()";
            return;
        }
        //初始化变量
        dbp = new DBProcessor("spider",dbPath);

        if(!Conf.status){
            qDebug()<<"No valid configuration data";
            return;
        }
        //初始化爬虫
        int maxRowId = dbp->getMaxId("data");
        qDebug()<<"Max row id is: "<<maxRowId;
        int errorNum =0;//出错计数，连续出错十次就停止爬虫
        int count=0;//成功计数
        bool status = false;//是否获取到第一个题目
        int currentNum = Conf.begin;//当前正在处理的ID，从第一题开始
        qDebug()<<Stopped;
        do{
            QString platformUrl = Conf.url + QString::number(currentNum);
            qDebug()<<"Spider ->platform url is: "<<platformUrl;
            QString context = getPlatformCtx(platformUrl);
            if(context.isEmpty()){
                errorNum++;
                currentNum++;
                continue;
            }
            else{
                //保存数据到数据库
                sql = "insert into data values(:id,:platform,:name,:question,"
                      ":answer,:tip,:cata,:level,:submit,:passed,:rate,:language,:status)";
                dbp->query->prepare(sql);
                //id部分
                dbp->query->bindValue(":id",maxRowId+1);
                //平台名称
                dbp->query->bindValue(":platform",cntPlatformName);
                //题目名字
                QString name = filterHtmlText(context,Conf.name);
                dbp->query->bindValue(":name",name);
                //题目详细数据
                QString detail = filterHtmlText(context,Conf.detail);
                if(detail.isEmpty()){
                    sleep(500);
                    currentNum++;//获取下一题
                    errorNum++;
                    continue;
                }
                dbp->query->bindValue(":question",detail);
                //题目答案
                dbp->query->bindValue(":answer","N/A");
                //提示
                dbp->query->bindValue(":tip","N/A");
                //难度等级
                QString level = filterHtmlText(context,Conf.level);
                dbp->query->bindValue(":level",level);
                QString cata = filterHtmlText(context,Conf.type);
                dbp->query->bindValue(":cata",cata);
                //题目分类
                dbp->query->bindValue(":type","OJ");
                //题目描述语言
                QString language=filterHtmlText(context,Conf.language);
                dbp->query->bindValue(":language",language);
                //提交数
                QString submit = filterHtmlText(context,Conf.submit);
                dbp->query->bindValue(":submit",submit.toInt());
                //通过数
                QString passed = filterHtmlText(context,Conf.passed);
                dbp->query->bindValue(":passed",passed.toInt());
                //通过率
                QString rate = filterHtmlText(context,Conf.rate);
                dbp->query->bindValue(":rate",rate);
                if(!dbp->query->exec()){
                    qDebug()<<"Cannot insert data into database: "<<dbp->query->lastError();
                    return;
                }
                else{
                    qDebug()<<"Insert data successfully";
                    qDebug()<<"name is: "<<name<<" platform is: "<<cntPlatformName<<" id is: "<<maxRowId;
                    QList<QStandardItem *> rowList;
                    rowList<<new QStandardItem(name)<<new QStandardItem(cntPlatformName)<<new QStandardItem(QString::number(maxRowId));
                    emit updateModel(rowList);
                    //model->appendRow(rowList);
                }
                //至此，数据保存完成
                status =true;
                errorNum=0;
                sleep(2000);
                currentNum++;
                count++;
                maxRowId++;
                emit countChanged(count);
            }
            if((count%49==0||count%75==0||count%126==0)&&status){
                sleep(4000);
            }
            //如果错误题目数大于30就停止爬虫
        }while(errorNum<=100&&!Stopped);
        qDebug()<<"All question retrive done";
        emit displayMsg(QString("All question retrive done"));

        if(Stopped){
            qDebug()<<"Spider has been stopped successfully";
            emit displayMsg(QString("Spider has been stopped successfully"));
        }
    }

    //停止爬虫运行
    void stop(){
        Stopped = true;
        reply->deleteLater();
        manager->deleteLater();
        qDebug()<<"Spider has been stopped";
    }

private:
    //加载爬虫浏览器的模拟头
    void loadUserAgent(){
        int i=0;
        QFile file(":/resources/user_agents.ini");
        if(file.open(QIODevice::ReadOnly)){
            QTextStream readIn(&file);
            while(!readIn.atEnd()){
                UserAgent[i++]=readIn.readLine();
            }
        }
        file.close();
    }

    //根据正则表达式过滤文本
    QString filterHtmlText(QString source,QString regText)
    {
        QString result;
        int pos=0;
        QRegExp reg(regText);
        if(source.isEmpty()|reg.isEmpty()) return "N/A";
        reg.setMinimal(true);
        while((pos=reg.indexIn(source,pos))!= -1){
            result += reg.cap(1)+" ";
            pos += reg.matchedLength();
        }
        return result;
    }

    //获取给定url的网页数据
    QString getPlatformCtx(QString url)
    {
        QString source;//网页结果
        //******模拟浏览器****************
        request.setUrl(url);
        request.setRawHeader("User-Agent",UserAgent[randomNum(900)].toLatin1());
        request.setRawHeader("Keep-Alive","300");
        request.setRawHeader("Connection","keep-alive");
        request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
        request.setRawHeader("Accept-Language","zh-CN,en-US,zh,en;q=0.9");
        request.setRawHeader("Accept-Encoding","deflate");
        request.setRawHeader("Referer","https://www.baidu.com/");
        //*******模拟结束******************
        reply = manager->get(request);
        //循环直到获取网页所有内容
        QObject::connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        //判断网页是否存在，是否有效
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if(statusCode==200){
            QByteArray ba = reply->readAll();
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            source = codec->toUnicode(ba);
            if(source.isEmpty())    return source;//没有数据就退出

            QString charset;
            int pos=0;
            QRegExp reg("charset=[\"](.+)\"[\\s]>");
            reg.setMinimal(true);
            while((pos=reg.indexIn(source,pos))!= -1){
                charset = reg.cap(1);
                pos += reg.matchedLength();
            }
            charset.remove('\"');
            if(!charset.isEmpty()){
                QTextCodec *codec = QTextCodec::codecForName(charset.toLocal8Bit());
                source = codec->toUnicode(ba).toLocal8Bit();
            }

            //如果网页内容中包含结束标志，则清空网页源文本
            foreach(QString mark,endMark){
                if(source.contains(mark)){
                    source.clear();
                }
            }
        }
        else{
            //如果网页获取不正常，则清空网页内容
            source.clear();
        }

        return source;
    }

signals:
    void countChanged(int count);
    void displayMsg(QString msg);
    void updateModel(QList<QStandardItem *> rowList);

public slots:
    void getPlatformName(QString name){
        cntPlatformName = name;
    }
};

#endif // SPIDER_HPP
