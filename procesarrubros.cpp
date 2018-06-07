#include "procesarrubros.h"
#include <Utilidades/database.h>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <Utilidades/logs.h>
#include <QDebug>
#include "QProcess"
#include <Utilidades/configuracionxml.h>

ProcesarRubros::ProcesarRubros(){}



bool ProcesarRubros::cargaDeListaDeRubros(){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoRubro,descripcionRubro FROM Rubros where  sincronizadoWeb='0'")){

        if(query.first()){
            query.previous();
            while(query.next()){
                if(ProcesarRubros::insertoListaRubros(query.value(0).toString(),query.value(1).toString()))
                    ProcesarRubros::marcarRubrosComoSincronizado(query.value(0).toString());

            }
            return true;
        }else{
            Logs::loguear("No existen rubros para cargar o actualizar");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoRubro...");
        return false;
    }
}



bool ProcesarRubros::insertoListaRubros(QString _codigoRubro,QString _descripcionRubro){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));

    if(existeRubroEnWeb(_codigoRubro)){
        if(query.exec("update rubro set nombre='"+_descripcionRubro+"' where id_dw='"+_codigoRubro+"'")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo actualizar el rubro: "+_codigoRubro);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }else{
        if(query.exec("INSERT INTO rubro(id_dw ,nombre)values('"+_codigoRubro+"','"+_descripcionRubro+"')")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo insertar el rubro: "+_codigoRubro);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }
}
bool ProcesarRubros::existeRubroEnWeb(QString _codigoRubro){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));
    if(query.exec("select * from rubro where id_dw='"+_codigoRubro+"'")){
        if(query.first()){
            return true;
        }else{
            return false;
        }
    }else{
        Logs::loguear("ERROR: No se pudo consultar la existencia del rubro: "+_codigoRubro);
        Logs::loguear(query.lastError().text());
        return false;
    }
}
void ProcesarRubros::marcarRubrosComoSincronizado(QString _codigoRubro){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));
    if(query.exec("UPDATE Rubros SET sincronizadoWeb='1'  where  codigoRubro='"+_codigoRubro+"';")){

    }else{
        Logs::loguear("Khitomer >> ERROR: No se pudo actualizar el rubro: "+_codigoRubro);
        Logs::loguear(query.lastError().text());
    }
}
