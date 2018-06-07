#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "QSqlQuery"


class Database : public QSqlDatabase
{
public:

    static QSqlDatabase connect(QString);
    static void closeDb();
    static QSqlQuery consultaSql(QString,QString);
   static bool cehqueStatusAccesoMysql(QString);


private:
   Database();


};

#endif // DATABASE_H
