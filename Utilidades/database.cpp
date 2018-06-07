#include "database.h"
#include "QSqlQuery"
#include "QSqlRecord"
#include <Utilidades/configuracionxml.h>
#include <QSqlError>

static QSqlDatabase dbconLocal= QSqlDatabase::addDatabase("QMYSQL","Local");
static QSqlDatabase dbcon= QSqlDatabase::addDatabase("QMYSQL","Remota");
static QSqlQuery resultadoConsulta;
static QSqlRecord rec;

Database::Database()
{
}


bool Database::cehqueStatusAccesoMysql(QString _instancia) {
    if(!Database::connect(_instancia).isOpen()){
        if(!Database::connect(_instancia).open()){
            return false;
        }else{
            QSqlQuery query = Database::consultaSql("select 1","local");
            if(query.first()) {
                return true;
            }else{
                Database::connect(_instancia).close();
                return false;
            }
        }
    }else{
        QSqlQuery query = Database::consultaSql("select 1;","local");
        if(query.first()) {
            return true;
        }else{
            Database::connect(_instancia).close();
            return false;
        }
    }
}



QSqlDatabase Database::connect(QString _instancia){

    if(_instancia=="local"){
        dbconLocal.setPort(ConfiguracionXml::getPuertoLocal());
        dbconLocal.setHostName(ConfiguracionXml::getHostLocal());
        dbconLocal.setDatabaseName(ConfiguracionXml::getBaseLocal());
        dbconLocal.setUserName(ConfiguracionXml::getUsuarioLocal());
        dbconLocal.setPassword(ConfiguracionXml::getClaveLocal());
        return dbconLocal;
    }else{
        dbcon.setPort(ConfiguracionXml::getPuertoRemoto());
        dbcon.setHostName(ConfiguracionXml::getHostRemoto());
        dbcon.setDatabaseName(ConfiguracionXml::getBaseRemoto());
        dbcon.setUserName(ConfiguracionXml::getUsuarioRemoto());
        dbcon.setPassword(ConfiguracionXml::getClaveRemoto());
        return dbcon;
    }
}
void Database::closeDb() {
    QSqlDatabase::removeDatabase("QMYSQL");
}

QSqlQuery Database::consultaSql(QString _consulta,QString _instancia){

    if(_instancia=="local"){
        resultadoConsulta = dbconLocal.exec(_consulta);
    }else{
        resultadoConsulta = dbcon.exec(_consulta);
    }
    resultadoConsulta = dbcon.exec(_consulta);
    rec = resultadoConsulta.record();
    return resultadoConsulta;
}
