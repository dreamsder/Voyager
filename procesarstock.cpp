#include "procesarstock.h"
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

ProcesarStock::ProcesarStock(){}


/// Carga el stock de los artículos
bool ProcesarStock::cargarStock(){
    Database::cehqueStatusAccesoMysql("remota");
    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));

    QString _replace="SET FOREIGN_KEY_CHECKS = 0; REPLACE INTO productos(id_dw,sub_rubro,stock,portada,visible,cuotas,recargo) values";
    int i=0;

    QString _stockAGuardar="0";
    qlonglong _stock=0;

    if(query.exec("SELECT id_dw,sub_rubro,portada,visible,cuotas,recargo FROM productos;")){
        if(query.first()){
            query.previous();

            while (query.next()) {
                _stockAGuardar="0";
                _stock=ProcesarStock::retornaStock(query.value(0).toString());

                if(_stock!=-999999){
                    if(_stock>0){
                        _stockAGuardar=QString::number(_stock);
                    }
                }else{
                    return false;
                }

                if(i==0){
                    _replace.append("('"+query.value(0).toString()+"','"+query.value(1).toString()+"','"+_stockAGuardar+"','"+query.value(2).toString()+"','"+query.value(3).toString()+"','"+query.value(4).toString()+"','"+query.value(5).toString()+"')");
                    i=1;
                }else{
                   _replace.append(",('"+query.value(0).toString()+"','"+query.value(1).toString()+"','"+_stockAGuardar+"','"+query.value(2).toString()+"','"+query.value(3).toString()+"','"+query.value(4).toString()+"','"+query.value(5).toString()+"')");
                }

            }

            _replace.append("; SET FOREIGN_KEY_CHECKS = 1;");

            if(ProcesarStock::actualizarStock(_replace)){
                return true;
            }else{
                return false;
            }

        }else{
            Logs::loguear("No existen artículos a los cuales actualizarle el stock.");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta de carga de stock:  SELECT id_dw...");
        Logs::loguear(query.lastError().text());
        return false;
    }
}


bool ProcesarStock::actualizarStock(QString _consultaSqlReplace){

  //  Logs::loguear(_consultaSqlReplace);

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));

    if(query.exec(_consultaSqlReplace)){

        return true;

    }else{
        Logs::loguear("ERROR: No se pudo actualizar el stock : "+_consultaSqlReplace);
        Logs::loguear(query.lastError().text());
        return false;
    }
}

qlonglong ProcesarStock::retornaStock(QString _codigoArticulo){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return -999999;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT AR.codigoArticulo, (SELECT sum(case when TDOC.afectaStock=1 then DOCL.cantidad else (DOCL.cantidad*-1) end)  FROM Documentos DOC  join DocumentosLineas DOCL on DOCL.codigoDocumento=DOC.codigoDocumento and DOCL.codigoTipoDocumento=DOC.codigoTipoDocumento   join TipoDocumento TDOC on TDOC.codigoTipoDocumento=DOC.codigoTipoDocumento   where TDOC.afectaStock!=0  and DOC.codigoEstadoDocumento in ('E','G','P') and DOC.fechaHoraGuardadoDocumentoSQL>= (SELECT fechaHoraGuardadoDocumentoSQL FROM Documentos where codigoTipoDocumento=8 and codigoEstadoDocumento in ('E','G') order by codigoDocumento desc limit 1) and DOCL.codigoArticulo=AR.codigoArticulo  group by DOCL.codigoArticulo)   FROM Articulos AR where AR.codigoArticulo='"+_codigoArticulo+"' and AR.tipoCliente=2 and AR.activo=1")){

        if(query.first()){

            return query.value(1).toInt();

        }else{
          //  Logs::loguear("No existe el artículo "+_codigoArticulo+" en la base de datos khitomer o se encuentra inactivo.");
            return 0;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT AR.codigoArticulo...");
        return -999999;
    }
}

