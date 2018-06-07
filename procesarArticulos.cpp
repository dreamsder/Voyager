#include "procesarArticulos.h"
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

ProcesarArticulos::ProcesarArticulos(){}


bool ProcesarArticulos::cargaDeArticulos(){

    Database::cehqueStatusAccesoMysql("local");


    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT AR.codigoArticulo,AR.descripcionArticulo,AR.codigoSubRubro,AR.activo  FROM Articulos AR where AR.sincronizadoWeb=0 and AR.tipoCliente=2")){
        QSqlRecord rec = query.record();
        if(query.first()){
            query.previous();
            while(query.next()){
                /// Chequeo si el articulo ya existe en la web, si existe, lo actualizo, si no existe, lo creo.
                if(ProcesarArticulos::existeArticuloEnBaseWeb(query.value(0).toString())){

                  //  Logs::loguear("Procedo a actualizar el artículo: "+query.value(0).toString());
                    if(ProcesarArticulos::actualizoArticulo(query.value(0).toString(),query.value(2).toString(),query.value(3).toString())){

                        ProcesarArticulos::actualizoArticuloDescripcion(query.value(0).toString(),query.value(1).toString());

                        //Logs::loguear("Marco el artículo como sincronizado: "+query.value(0).toString());
                        ProcesarArticulos::marcarArticuloComoSincronizado(query.value(0).toString());
                    }else{
                        Logs::loguear("No se pudo actualizar el artículo: "+query.value(0).toString());
                        Logs::loguear("Se continua con el siguiente artículo");
                    }

                }else{
                  //  Logs::loguear("Procedo a insertar el artículo: "+query.value(0).toString());
                    if(ProcesarArticulos::insertoArticulo(query.value(0).toString(),query.value(2).toString(),query.value(3).toString())){

                        ProcesarArticulos::insertoArticuloDescripcion(query.value(0).toString(),query.value(1).toString());

                  //      Logs::loguear("Marco el artículo como sincronizado: "+query.value(0).toString());
                        ProcesarArticulos::marcarArticuloComoSincronizado(query.value(0).toString());
                    }else{
                        Logs::loguear("No se pudo insertar el artículo: "+query.value(0).toString());
                        Logs::loguear("Se continua con el siguiente artículo");
                    }
                }
            }
            return true;
        }else{
            Logs::loguear("No existen artículos para cargar o actualizar");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoArticulo...");
        return false;
    }
}






bool ProcesarArticulos::existeArticuloEnBaseWeb(QString _codigoArticulo){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(query.exec("select id_dw from productos where id_dw='"+_codigoArticulo+"'")){
        if(query.first()){
            return true;
        }else{
            return false;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT id_dw FROM productos...: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
        return false;
    }
}

bool ProcesarArticulos::actualizoArticulo(QString _codigoArticulo, QString _codigoSubRubro,QString _activo){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(query.exec("UPDATE  productos set sub_rubro='"+_codigoSubRubro+"',visible='"+_activo+"'   where id_dw='"+_codigoArticulo+"'")){
        return true;
    }else{
        Logs::loguear("ERROR: No se pudo actualizar el artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
        return false;
    }
}

bool ProcesarArticulos::actualizoArticuloDescripcion(QString _codigoArticulo, QString _nombreArticulo){


    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(query.exec("UPDATE  productos_descripcion set nombre='"+_nombreArticulo+"'  where id_dw='"+_codigoArticulo+"'")){
        return true;
    }else{
        Logs::loguear("ERROR: No se pudo actualizar la descripcion del artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
        return false;
    }
}


bool ProcesarArticulos::insertoArticulo(QString _codigoArticulo, QString _codigoSubRubro, QString _activo){


    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(query.exec("REPLACE INTO productos(id_dw,sub_rubro,visible)values('"+_codigoArticulo+"','"+_codigoSubRubro+"','"+_activo+"')")){
        return true;
    }else{
        Logs::loguear("ERROR: No se pudo insertar el artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
        return false;
    }
}

bool ProcesarArticulos::insertoArticuloDescripcion(QString _codigoArticulo, QString _nombreArticulo){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(query.exec("REPLACE INTO productos_descripcion(id_dw,nombre)values('"+_codigoArticulo+"','"+_nombreArticulo+"')")){
        return true;
    }else{
        Logs::loguear("ERROR: No se pudo insertar la descripción del artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
        return false;
    }
}

void ProcesarArticulos::marcarArticuloComoSincronizado(QString _codigoArticulo){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));
    if(query.exec("UPDATE Articulos SET sincronizadoWeb='1' where codigoArticulo='"+_codigoArticulo+"';")){

    }else{
        Logs::loguear("Khitomer >> ERROR: No se pudo actualizar el artículo: "+_codigoArticulo);
        Logs::loguear(query.lastError().text());
    }
}




/*
/////////
//Funcion para ejecutar comandos por consola
/////////
int ProcesarArticulos::EjectuarComando(QString comando){
    QStringList arg;
    QProcess procesos;
    QString p = "bash";
    arg << "-c"<< comando;
    int ok = procesos.execute(p,arg);
    return ok;
}


/////////
// Informe de error en la carga de infomadai
/////////
void ProcesarArticulos::informeDeCargaDeDatosInfoMadai(QString _mensaje){

    QString reclamosAInformar="echo -e \""+_mensaje+" \\n\\n";

    reclamosAInformar.append("\"| mail -r CargadorMonitoreo@sisteco.com.uy -s \"Cargador MONITOREO de reclamos infoMadai: informe de << ERROR >> "+QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss")+"\" jvila@sisteco.com.uy,gzipitria@sisteco.com.uy,cmontano@sisteco.com.uy");
    if(ProcesarArticulos::EjectuarComando(reclamosAInformar)==0)
    {
        Logs::loguear( "Informe de << ERROR >> enviado correctamente ");
    }else{
        Logs::loguear( "Función: informeDeCargaDeDatosInfoMadai == No se pudo enviar el informe de error por mail ");
    }
}
*/
