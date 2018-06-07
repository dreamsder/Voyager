#include "procesarclientesdesdekhitomer.h"
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


ProcesarClientesDesdeKhitomer::ProcesarClientesDesdeKhitomer(){}

/// Carga las actualizaciones de lista de precios de clientes desde khitomer hacia la web
bool ProcesarClientesDesdeKhitomer::cargaDeClientes(){

    Database::cehqueStatusAccesoMysql("local");


    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoCliente,rut FROM Clientes where  esClienteWeb='1' and  sincronizadoWeb='0' and tipoCliente=1")){
        if(query.first()){
            query.previous();
            while (query.next()) {

                if(ProcesarClientesDesdeKhitomer::actulizoClientesWeb(query.value(0).toString(),query.value(1).toString())){
                    ProcesarClientesDesdeKhitomer::marcarClienteComoSincronizado(query.value(0).toString());
                }

            }
            return true;
        }else{
            Logs::loguear("No existen clientes para actualizar en la web.");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta de carga de clientes:  SELECT codigoCliente...");
        Logs::loguear(query.lastError().text());
        return false;
    }
}


bool ProcesarClientesDesdeKhitomer::actulizoClientesWeb(QString _codigoCliente,QString _rut){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));

    QString _listaPrecio=ProcesarClientesDesdeKhitomer::retornaListaPrecioCliente(_codigoCliente);

    if(_listaPrecio.trimmed()!=""){
        if(query.exec("UPDATE  usuarios SET lista_precio='"+_listaPrecio+"'  where  id_dw='"+_codigoCliente+"' ")){
            return true;

        }else{
            Logs::loguear("ERROR: No se pudo ejecutar la consulta de actualización de clientes web:  SELECT email...");
            Logs::loguear("Cliente: "+_codigoCliente);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }else{
        return false;
    }
}



QString ProcesarClientesDesdeKhitomer::retornaListaPrecioCliente(QString _codigoCliente){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }

    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT LPC.codigoListaPrecio FROM ListaPrecioClientes LPC  join ListaPrecio LP on LP.codigoListaPrecio=LPC.codigoListaPrecio  where LPC.codigoCliente='"+_codigoCliente+"' and LPC.TipoCliente=1 and LP.vigenciaHastaFecha>CURRENT_DATE() order by LP.vigenciaHastaFecha desc limit 1")){
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toString();
            }else{
                return "";
            }
        }else{
            return "";
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
        return "";
    }
}

void ProcesarClientesDesdeKhitomer::marcarClienteComoSincronizado(QString _codigoCliente){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));
    if(query.exec("UPDATE Clientes SET sincronizadoWeb='1'  where codigoCliente='"+_codigoCliente+"' and tipoCliente=1;")){

    }else{
        Logs::loguear("Khitomer >> ERROR: No se pudo actualizar el cliente: "+_codigoCliente);
        Logs::loguear(query.lastError().text());
    }
}
