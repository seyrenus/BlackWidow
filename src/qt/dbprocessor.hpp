#ifndef DBPROCESSOR_HPP
#define DBPROCESSOR_HPP

#include "base.hpp"

class DBProcessor{
public:
    QSqlQuery *query;
    QSqlTableModel *model;

private:
    QSqlDatabase db;
    QString sql;
    QString dbFilePath;
public:
    DBProcessor(QString cntName,QString dbPath){
        qDebug()<<"DBProcessor::DBProcessor()";
        qDebug()<<"Current db connection name is: "<<cntName
               <<"\ndb path is: "<<dbPath;
        dbFilePath = dbPath;
        db = QSqlDatabase::addDatabase("QSQLITE",cntName);
        db.setDatabaseName(dbPath);
        if(db.open()){
            qDebug()<<"Connect to db: "<<dbPath;
        }
        else{
            qDebug()<<"Cannot connect to db: "<<dbPath;
        }
        query = new QSqlQuery(db);
    }

    ~DBProcessor(){
        qDebug()<<"DBProcessor::~DBProcessor()";
        delete query;
        db.close();
    }
//重置数据库，如果数据库存在则先删除，否则直接生成新数据库文件。
    bool reset(){
        sql.clear();
        QFile file(":/resources/init.sql");
        if(file.open(QIODevice::Text|QIODevice::ReadOnly)){
            QTextStream readIn(&file);
            sql = readIn.readAll();
            qDebug()<<"Initialize file loaded";
        }
        else{
            qDebug()<<"Cannot read sql initialization file";
            return false;
        }
        file.close();

        file.setFileName(dbFilePath);
        if(file.exists()) file.remove();
        //生成新数据库
        db.setDatabaseName(dbFilePath);
        db.open();
        if(query->exec(sql)){
            qDebug()<<"Reset db successfully";
            return true;
        }
        else{
            qDebug()<<"Cannot reset db: "<<query->lastError();
            return false;
        }

        return true;
    }

    int getMaxId(QString tableName){
        int id=-1;
        sql = QString("select max(id) from %1").arg(tableName);
        query->prepare(sql);
        if(query->exec()){
            while(query->next()){
                id = query->value(0).toInt();
            }
        }
        else{
            qDebug()<<"Cannot query max id from table: "<<tableName;
            return -1;
        }
        return id;
    }

    QStringList getTableList(QString tableName){
        QStringList resultList;
        query->prepare(QString("select value from %1").arg(tableName));
        if(query->exec()){
            while(query->next()){
                resultList += query->value(0).toString();
            }
        }
        else{
            qDebug()<<"Cannot query data from database: "<<query->lastError();
            return resultList;
        }
        return resultList;
    }
};

#endif // DBPROCESSOR_HPP
