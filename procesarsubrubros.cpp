#include "procesarsubrubros.h"
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

ProcesarSubRubros::ProcesarSubRubros(){}



bool ProcesarSubRubros::cargaDeListaDeSubRubros(){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoSubRubro,codigoRubro,descripcionSubRubro FROM SubRubros where sincronizadoWeb='0'")){
        QSqlRecord rec = query.record();
        if(query.first()){
            query.previous();
            while(query.next()){

                if(ProcesarSubRubros::insertoListaSubRubros(query.value(0).toString(),query.value(1).toString(),query.value(2).toString()))
                    ProcesarSubRubros::marcarSubRubrosComoSincronizado(query.value(1).toString(),query.value(0).toString());

            }
            return true;
        }else{
         //   Logs::loguear("No existen subrubros para cargar o actualizar");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoSubRubro...");
        return false;
    }
}



bool ProcesarSubRubros::insertoListaSubRubros(QString _codigoSubRubro,QString _codigoRubro,QString _descripcionSubRubro){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }    

    QSqlQuery query(Database::connect("remota"));

    if(existeSubRubroEnWeb(_codigoRubro,_codigoSubRubro)){
        if(query.exec("update sub_rubro set nombre='"+_descripcionSubRubro+"' where id_dw='"+_codigoSubRubro+"' and id_padre='"+_codigoRubro+"'")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo actualizar el subrubro: "+_codigoSubRubro+" - rubro:"+_codigoRubro);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }else{
        if(query.exec("INSERT INTO sub_rubro(id_dw ,id_padre,nombre)values('"+_codigoSubRubro+"','"+_codigoRubro+"','"+_descripcionSubRubro+"')")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo insertar el subrubro: "+_codigoSubRubro+" - rubro:"+_codigoRubro);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }

}
bool ProcesarSubRubros::existeSubRubroEnWeb(QString _codigoRubro,QString _codigoSubRubro){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));
    if(query.exec("select * from sub_rubro where id_dw='"+_codigoSubRubro+"' and id_padre='"+_codigoRubro+"'")){
        if(query.first()){
            return true;
        }else{
            return false;
        }
    }else{
        Logs::loguear("ERROR: No se pudo consultar la existencia del sub rubro: "+_codigoRubro);
        Logs::loguear(query.lastError().text());
        return false;
    }
}
void ProcesarSubRubros::marcarSubRubrosComoSincronizado(QString _codigoRubro,QString _codigoSubRubro){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));
    if(query.exec("UPDATE SubRubros SET sincronizadoWeb='1' where codigoSubRubro='"+_codigoSubRubro+"' and codigoRubro='"+_codigoRubro+"';")){

    }else{
        Logs::loguear("Khitomer >> ERROR: No se pudo actualizar el sub rubro: "+_codigoSubRubro);
        Logs::loguear(query.lastError().text());
    }
}

