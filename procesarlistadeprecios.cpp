#include "procesarlistadeprecios.h"
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

ProcesarListaDePrecios::ProcesarListaDePrecios(){}



bool ProcesarListaDePrecios::cargaDeListaDePrecios(){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT LPA.codigoListaPrecio,LPA.codigoArticulo,round(case when AR.codigoMoneda=1 then LPA.precioArticulo/(select MONE.cotizacionMoneda from Monedas MONE where MONE.codigoMoneda=2 limit 1) else LPA.precioArticulo end,2) FROM ListaPrecioArticulos LPA left join Articulos AR on AR.codigoArticulo=LPA.codigoArticulo left join Monedas MON on MON.codigoMoneda=AR.codigoMoneda  where AR.activo='1' and LPA.sincronizadoWeb='0' ")){

        if(query.first()){
            query.previous();
            while(query.next()){

                if(ProcesarListaDePrecios::insertoListaPrecio(query.value(0).toString(),query.value(1).toString(),query.value(2).toDouble()))
                    ProcesarListaDePrecios::marcarListaPreciosComoSincronizado(query.value(0).toString(),query.value(1).toString());

            }
            return true;
        }else{
            Logs::loguear("No existen listas de precio para cargar o actualizar");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoListaPrecio...");
        return false;
    }
}



bool ProcesarListaDePrecios::insertoListaPrecio(QString _codigoListaPrecio,QString _codigoArticulo, double _precioArticulo){

    Database::cehqueStatusAccesoMysql("remota");


    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));

    QString _precioASetear=QString::number(_precioArticulo);
    if(existeListaPrecioEnWeb(_codigoListaPrecio,_codigoArticulo)){



        if(query.exec("update listas_precios set precio='"+_precioASetear+"'  where id_lista='"+_codigoListaPrecio+"' and id_producto='"+_codigoArticulo+"'")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo actualizar el precio: "+_codigoListaPrecio +" - articulo:"+_codigoArticulo);
            Logs::loguear(query.lastError().text());
            return false;
        }

    }else{
        if(query.exec("INSERT INTO listas_precios(id_lista,id_producto,precio)values('"+_codigoListaPrecio+"','"+_codigoArticulo+"','"+_precioASetear+"')")){
            return true;
        }else{
            Logs::loguear("ERROR: No se pudo insertar el artículo: "+_codigoArticulo+", lista de precio: "+_codigoListaPrecio);
            Logs::loguear(query.lastError().text());
            return false;
        }
    }


}

bool ProcesarListaDePrecios::existeListaPrecioEnWeb(QString _codigoListaPrecio,QString _codigoArticulo){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));
    if(query.exec("select * from listas_precios where id_lista='"+_codigoListaPrecio+"' and id_producto='"+_codigoArticulo+"'")){
        if(query.first()){
            return true;
        }else{
            return false;
        }
    }else{
        Logs::loguear("ERROR: No se pudo consultar la existencia de la lista de precio: "+_codigoListaPrecio);
        Logs::loguear(query.lastError().text());
        return false;
    }
}
void ProcesarListaDePrecios::marcarListaPreciosComoSincronizado(QString _codigoListaPrecio,QString _codigoArticulo){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));
    if(query.exec("UPDATE ListaPrecioArticulos SET sincronizadoWeb='1'  where codigoListaPrecio='"+_codigoListaPrecio+"' and codigoArticulo='"+_codigoArticulo+"';")){

    }else{
        Logs::loguear("Khitomer >> ERROR: No se pudo actualizar la lista precio: "+_codigoListaPrecio +" - artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
    }
}

