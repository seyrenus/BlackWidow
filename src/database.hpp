#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "blackwidow.hpp"

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

#define MAX_P 0.25f
#define MAX_L 4

class Database : public QThread
{
public:
    Database(QString connectName="default",QString DBName="blackwidow.db")
    {
        db = QSqlDatabase::addDatabase("QSQLITE",connectName);
        db.setDatabaseName(DBName);
        if(!db.open()){
            qDebug()<<"Cannot open database file: "<<db.lastError();
        }
        else{
            qDebug()<<"Connect to database successfully";
        }
        query = new QSqlQuery(db);
        qDebug()<<"Database thread id is: "<<this->thread()->currentThreadId();
        model = new QSqlTableModel(this,db);
        model->setTable("task");
    }
    ~Database()
    {
        delete model;
        delete query;
        db.close();
    }
    QStringList getTableList(QString tableName)
    {
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

    int getMaxId(QString tableName)
    {
        int id=-1;
        QString maxIdSQL = "select max(id) from " + tableName;
        query->prepare(maxIdSQL);
        if(query->exec()){
            while(query->next()){
                id = query->value(0).toInt();
            }
        }
        else{
            qDebug()<<"Cannot query max id from table: "<<tableName<<query->lastError();
            return -1;
        }
        qDebug()<<"maxldSQL is: "<<maxIdSQL<<" value is: "<<id;
        return id;
    }
    bool word_segment(const QString &subStr)
    {
        Q_UNUSED(subStr)
        return true;
    }
    float Cosine(QString &baseStr, QString &tempStr)
    {
        QMap<QString,QPair<size_t,size_t>> container;
        for(size_t i=0,start = 0;i<static_cast<unsigned long>(baseStr.length());++i){
            QString subStr = baseStr.mid(static_cast<int>(start),static_cast<int>(i-start+1));
            if(word_segment(subStr)){
                ++container[subStr].first;
                start = i+1;
            }
        }
        for(size_t i=0,start =0;i<static_cast<unsigned long>(tempStr.length());++i){
            QString subStr = tempStr.mid(static_cast<int>(start),static_cast<int>(i-start+1));
            if(word_segment(subStr)){
                ++container[subStr].second;
                start = i+1;
            }
        }
        unsigned long product =0;
        unsigned long modulo1=0;
        unsigned long modulo2 =0;
        for(QMap<QString,QPair<size_t,size_t>>::iterator it=container.begin();it!=container.end();++it){
            QPair<size_t,size_t> cnt = it.value();
            product += cnt.first * cnt.second;
            modulo1 += cnt.first * cnt.first;
            modulo2 += cnt.second * cnt.second;
        }
        if(!product||!modulo1||!modulo2) return 0;
        return product/(sqrt(static_cast<float>(modulo1))*sqrt(static_cast<float>(modulo2)));
    }
    float Leven(const QString &baseStr, const QString &tempStr)
    {
        if(baseStr.isEmpty() || tempStr.isEmpty()) return 0;
        size_t distance = 0;
        size_t len=0;
        if(baseStr.length()<baseStr.length()){
            distance = compare(baseStr,tempStr);
            len = static_cast<size_t>(tempStr.length());
        }
        else{
            distance = compare(baseStr,tempStr);
            len= static_cast<size_t>(baseStr.length());
        }
        return distance < len?1-static_cast<float>(distance)/len:0;
    }
    size_t compare(const QString &baseStr, const QString &tempStr)
    {
        size_t baseLength = static_cast<size_t>(baseStr.length());
        size_t tempLength = static_cast<size_t>(tempStr.length());
        size_t **distance = new size_t*[baseLength+1];
        for (size_t i = 0; i < baseLength + 1; ++i){
            distance[i] = new size_t[tempLength + 1];
        }
        distance[0][0] = 0;
        for (size_t i = 1; i < baseLength + 1; ++i){
            distance[i][0] = distance[i - 1][0] + del(baseStr.at(static_cast<int>(i - 1)));
        }
        for (size_t i = 1; i < tempLength + 1; ++i){
            distance[0][i] = distance[0][i - 1] + ins(tempStr.at(static_cast<int>(i - 1)));
        }
        for (size_t i = 1; i < baseLength + 1; ++i){
            for (size_t j = 1; j < tempLength + 1; ++j){
                size_t alpha = distance[i][j - 1] + ins(tempStr.at(static_cast<int>(j - 1)));
                size_t delta = distance[i - 1][j] + del(baseStr.at(static_cast<int>(i - 1)));
                size_t gamma = distance[i - 1][j - 1] + sub(baseStr.at(static_cast<int>(i - 1)), tempStr.at(static_cast<int>(j - 1)));
                distance[i][j] = std::min(std::min(alpha, delta), gamma);
            }
        }
        return distance[baseLength][tempLength];
    }
    bool compare_char(QChar ch1,QChar ch2){
            return ch1==ch2;
    }
    size_t ins(QChar ch){
        Q_UNUSED(ch)
        return 1;
    }
    size_t del(QChar ch){
        Q_UNUSED(ch)
        return 1;
    }
    size_t sub(QChar ch1,QChar ch2){
            return compare_char(ch1,ch2);
    }
    void run()
    {
        if(columName.isEmpty()||tableName.isEmpty()||var.isNull()){
            resultValue = 0;
            return;
        }
        qDebug()<<"judge thread id is:"<<qApp->thread()->currentThreadId();
        sql = QString("select id,%1 from %2").arg(columName,tableName);
        query->prepare(sql);
        if(!query->exec()){
            qDebug()<<"Cannot query data: "<<query->lastError();
        }
        else{
            while(query->next()){
                QString key = var.toString();
                QString value = query->value(1).toString();
                float cosine=Cosine(key,value);
                float leven = Leven(key,value);
                float result =0.6*cosine +0.4*leven;
                if(result>0.94){
                    resultValue = query->value(0).toInt();
                }
            }
        }
        columName.clear();
        tableName.clear();
        var.clear();
        qDebug()<<"Two string are not same";
        resultValue = 0;
    }
public:
    QSqlQuery *query;
    QSqlDatabase db;
    QString sql;
    QString tableName;
    QString columName;
    QVariant var;
    int resultValue;
    QSqlTableModel *model;
};


#endif // DATABASE_HPP
