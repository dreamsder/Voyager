#include "procesarclientes.h"
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


ProcesarClientes::ProcesarClientes(){}


/// Carga los cliente de la web, hacia la base de Khitomer
bool ProcesarClientes::cargaDeClientes(){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));

    if(query.exec("SELECT email,nombre,apellido,rut,lista_precio, direccion, telefono, departamento, ciudad FROM usuarios where id_dw is null")){

        if(query.first()){
            query.previous();
            while (query.next()) {

                QString _direccionTotal=query.value(5).toString()+" "+query.value(7).toString()+" "+query.value(8).toString();

                QString _nuevoRut=query.value(3).toString();
                if(query.value(3).toString()=="0")
                    _nuevoRut="";

                ProcesarClientes::insertarClientes(query.value(0).toString(),query.value(1).toString(),query.value(2).toString(),_nuevoRut,query.value(4).toString(),_direccionTotal,query.value(6).toString());

            }
            return true;
        }else{
            Logs::loguear("No existen clientes para cargar.");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta de carga de clientes:  SELECT email...");
        Logs::loguear(query.lastError().text());
        return false;
    }
}


bool ProcesarClientes::insertarClientes(QString _email, QString _nombre, QString _apellido, QString _rut, QString _lista_precio,QString _direccion, QString _telefono){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(ProcesarClientes::existeClienteEnBaseKhitomer(_email,_rut)){

        return true;

    }else{
        query.clear();
        QString _ultimoNumeroCliente=ProcesarClientes::retornaUltimoCodigoClienteDisponible();

        if(_ultimoNumeroCliente=="")
            return false;

        if(query.exec("INSERT INTO Clientes(codigoCliente, tipoCliente, nombreCliente, razonSocial, rut, tipoClasificacion,documento,email,contacto,usuarioAlta,codigoPais,codigoDepartamento,codigoLocalidad,esClienteWeb,sincronizadoWeb,direccion,telefono) "
                      "values('"+_ultimoNumeroCliente+"',1,'"+_nombre+" "+_apellido+"','"+_apellido+"','"+_rut.trimmed()+"',1,'"+_rut+"','"+_email+"','"+_nombre+" "+_apellido+"','admin',1,1,1,'1','1','"+_direccion+"','"+_telefono+"')")){

            ProcesarClientes::actulizoClientesWeb(_ultimoNumeroCliente,_email,_rut);
            return true;

        }else{
            Logs::loguear("ERROR: No se pudo insertar el cliente en la base khitomer: "+_ultimoNumeroCliente);
            Logs::loguear(query.lastError().text());
            Logs::loguear(query.lastQuery());
            return false;
        }
    }
}


bool ProcesarClientes::existeClienteEnBaseKhitomer(QString _email,QString _rucCliente){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }

    QSqlQuery query(Database::connect("local"));

    /// Chequeo si el RUT no tiene un valor nulo o vacio. Si lo tiene, paso al chequeo 2.
    if(_rucCliente.isNull() || _rucCliente.trimmed()=="null" || _rucCliente.isEmpty() || _rucCliente.trimmed()==""){
        /// Chequeo 2: Chequea si el email tiene un valor nulo o vacio. Si es vacio retorno falso.
        if(_email.isNull() || _email.trimmed()=="null" || _email.isEmpty() || _email.trimmed()==""){
            return false;
        }else{

            /// Al tener el mail un valor valido, chequeo si existe el cliente con ese email.
            if(query.exec("SELECT codigoCliente FROM Clientes where tipoCliente=1  and email='"+_email+"' limit 1")){
                if(query.first()){

                    ProcesarClientes::actulizoClientesWeb(query.value(0).toString(),_email,_rucCliente);
                    return true;

                }else{
                    Logs::loguear("No existe el cliente: "+_email +" rut: "+_rucCliente);
                    return false;
                }
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
                return false;
            }
        }
    }else{
        /// Al tener un rut valido, chequeo si existe el cliente con ese rut
        if(query.exec("SELECT codigoCliente FROM Clientes where tipoCliente=1  and rut='"+_rucCliente+"'   limit 1")){
            if(query.first()){

                ProcesarClientes::actulizoClientesWeb(query.value(0).toString(),_email,_rucCliente);
                return true;

            }else{
                query.clear();

                /// Si no existe el cliente con ese rut, chequeo si existe con el email
                if(query.exec("SELECT codigoCliente FROM Clientes where tipoCliente=1  and email='"+_email+"' limit 1")){
                    if(query.first()){

                        ProcesarClientes::actulizoClientesWeb(query.value(0).toString(),_email,_rucCliente);
                        return true;

                    }else{
                        Logs::loguear("No existe el cliente: "+_email +" rut: "+_rucCliente);
                        return false;
                    }
                }else{
                    Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
                    return false;
                }

            }
        }else{
            Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
            return false;
        }
    }





}




bool ProcesarClientes::actulizoClientesWeb(QString _codigoCliente,QString _email,QString _rut){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }
    QSqlQuery query(Database::connect("remota"));
    if(_rut.isEmpty() || _rut.isNull() || _rut.trimmed()=="" || _rut.trimmed()=="null"){
        QString _listaPrecio=ProcesarClientes::retornaListaPrecioCliente(_codigoCliente);

        if(_listaPrecio.trimmed()=="" || _listaPrecio.isEmpty() || _listaPrecio.isNull()){
            if(query.exec("UPDATE  usuarios SET id_dw='"+_codigoCliente+"',lista_precio='1'  where  email='"+_email+"' and  id_dw is null")){
                return true;

            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta de actualización de clientes web:  SELECT email...");
                Logs::loguear("Cliente: "+_codigoCliente+"  - Email: "+_email);
                Logs::loguear(query.lastError().text());
                return false;
            }
        }else{
            if(query.exec("UPDATE  usuarios SET id_dw='"+_codigoCliente+"',lista_precio='"+_listaPrecio+"'  where  email='"+_email+"' and  id_dw is null")){
                return true;

            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta de actualización de clientes web:  SELECT email...");
                Logs::loguear("Cliente: "+_codigoCliente+"  - Email: "+_email);
                Logs::loguear(query.lastError().text());
                return false;
            }
        }



    }else{

        QString _listaPrecio=ProcesarClientes::retornaListaPrecioCliente(_codigoCliente);

        if(_listaPrecio.trimmed()=="" || _listaPrecio.isEmpty() || _listaPrecio.isNull()){
            if(query.exec("UPDATE  usuarios SET id_dw='"+_codigoCliente+"'  where  email='"+_email+"' and  id_dw is null")){
                return true;
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta de actualización de clientes web:  SELECT email...");
                Logs::loguear("Cliente: "+_codigoCliente+"  - Email: "+_email);
                Logs::loguear(query.lastError().text());
                return false;
            }
        }else{
            if(query.exec("UPDATE  usuarios SET id_dw='"+_codigoCliente+"' ,lista_precio='"+_listaPrecio+"'  where  email='"+_email+"' and  id_dw is null")){
                return true;
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta de actualización de clientes web:  SELECT email...");
                Logs::loguear("Cliente: "+_codigoCliente+"  - Email: "+_email);
                Logs::loguear(query.lastError().text());
                return false;
            }
        }
    }
}


QString ProcesarClientes::retornaListaPrecioCliente(QString _codigoCliente){

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
                return "1";
            }
        }else{
            return "";
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
        return "";
    }
}



QString ProcesarClientes::retornaUltimoCodigoClienteDisponible(){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }

    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoCliente FROM Clientes where tipoCliente=1 order by cast(codigoCliente as unsigned) desc limit 1")){
        if(query.first()){
            return QString::number(query.value(0).toInt()+1);
        }else{
            return "1";
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:  SELECT codigoCliente...");
        return "";
    }
}
