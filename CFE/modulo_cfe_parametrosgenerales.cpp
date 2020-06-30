/*********************************************************************
Khitomer - Sistema de facturación
Copyright (C) <2012-2018>  <Cristian Montano>

Este archivo es parte de Khitomer.

Khitomer es software libre: usted puede redistribuirlo y/o modificarlo
bajo los términos de la Licencia Pública General GNU publicada
por la Fundación para el Software Libre, ya sea la versión 3
de la Licencia, o (a su elección) cualquier versión posterior.

Este programa se distribuye con la esperanza de que sea útil, pero
SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
Consulte los detalles de la Licencia Pública General GNU para obtener
una información más detallada.

Debería haber recibido una copia de la Licencia Pública General GNU
junto a este programa.
En caso contrario, consulte <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "modulo_cfe_parametrosgenerales.h"
#include <QtSql>
#include <QSqlQuery>
#include <Utilidades/database.h>
//#include <QFileDialog>
#include <QDebug>
#include <Utilidades/logs.h>
#include <Utilidades/configuracionxml.h>



QString Modulo_CFE_ParametrosGenerales::m_modoTrabajoCFE;
QString Modulo_CFE_ParametrosGenerales::m_password;
QString Modulo_CFE_ParametrosGenerales::m_token;
QString Modulo_CFE_ParametrosGenerales::m_urlEfacturaContado;
QString Modulo_CFE_ParametrosGenerales::m_urlEfacturaCredito;
QString Modulo_CFE_ParametrosGenerales::m_urlEfacturaNotaCredito;
QString Modulo_CFE_ParametrosGenerales::m_urlEfacturaNotaDebito;
QString Modulo_CFE_ParametrosGenerales::m_urlEticketContado;
QString Modulo_CFE_ParametrosGenerales::m_urlEticketCredito;
QString Modulo_CFE_ParametrosGenerales::m_urlEticketNotaCredito;
QString Modulo_CFE_ParametrosGenerales::m_urlEticketNotaDebito;
QString Modulo_CFE_ParametrosGenerales::m_urlImixProduccion;
QString Modulo_CFE_ParametrosGenerales::m_urlImixTesting;
QString Modulo_CFE_ParametrosGenerales::m_urlWS;
QString Modulo_CFE_ParametrosGenerales::m_username;
QString Modulo_CFE_ParametrosGenerales::m_modoFuncionCFE;
QString Modulo_CFE_ParametrosGenerales::m_envioArticuloClienteGenerico;
QString Modulo_CFE_ParametrosGenerales::m_claveCifrada;

QString Modulo_CFE_ParametrosGenerales::m_urlDGI;


Modulo_CFE_ParametrosGenerales::Modulo_CFE_ParametrosGenerales(QObject *parent)
    : QAbstractListModel(parent)
{

    QHash<int, QByteArray> roles;
    roles[nombreParametroRole] = "nombreParametro";
    roles[valorParametroRole] = "valorParametro";

    setRoleNames(roles);
}


CFE_ParametrosGenerales::CFE_ParametrosGenerales(const QString &nombreParametro, const QString &valorParametro )
    : m_nombreParametro(nombreParametro), m_valorParametro(valorParametro)
{
}

QString CFE_ParametrosGenerales::nombreParametro() const
{
    return m_nombreParametro;
}
QString CFE_ParametrosGenerales::valorParametro() const
{
    return m_valorParametro;
}


void Modulo_CFE_ParametrosGenerales::agregar(const CFE_ParametrosGenerales &generico)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_generico << generico;
    endInsertRows();
}

void Modulo_CFE_ParametrosGenerales::limpiar(){
    m_generico.clear();
}

void Modulo_CFE_ParametrosGenerales::buscar(QString campo, QString datoABuscar){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery query(Database::connect("local"));


    if(query.exec("select * from CFE_ParametrosGenerales where "+campo+"'"+datoABuscar+"'")){

        QSqlRecord rec = query.record();

        Modulo_CFE_ParametrosGenerales::reset();
        if(query.record().count()>0){

            while (query.next()){
                Modulo_CFE_ParametrosGenerales::agregar(CFE_ParametrosGenerales(query.value(rec.indexOf("nombreParametro")).toString()
                                                                                , query.value(rec.indexOf("valorParametro")).toString()
                                                                                ));
            }
        }
    }else{
        Logs::loguear("Voyager >> ERROR: No se pudo ejecutar la consulta: Modulo_CFE_ParametrosGenerales::buscar");
        Logs::loguear(query.lastError().text());
    }
}

void Modulo_CFE_ParametrosGenerales::cargar(){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
        }
    }
    QSqlQuery q(Database::connect("local"));


    if(q.exec("select * from CFE_ParametrosGenerales;")){

        QSqlRecord rec = q.record();

        Modulo_CFE_ParametrosGenerales::reset();
        if(q.record().count()>0){

            while (q.next()){


                if(q.value(rec.indexOf("nombreParametro")).toString()=="modoTrabajoCFE"){Modulo_CFE_ParametrosGenerales::setmodoTrabajoCFE(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="password"){Modulo_CFE_ParametrosGenerales::setpassword(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="token"){Modulo_CFE_ParametrosGenerales::settoken(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-facturaContado"){Modulo_CFE_ParametrosGenerales::seturlEfacturaContado(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-facturaCredito"){Modulo_CFE_ParametrosGenerales::seturlEfacturaCredito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-facturaNotaCredito"){Modulo_CFE_ParametrosGenerales::seturlEfacturaNotaCredito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-facturaNotaDebito"){Modulo_CFE_ParametrosGenerales::seturlEfacturaNotaDebito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-ticketContado"){Modulo_CFE_ParametrosGenerales::seturlEticketContado(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-ticketCredito"){Modulo_CFE_ParametrosGenerales::seturlEticketCredito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-ticketNotaCredito"){Modulo_CFE_ParametrosGenerales::seturlEticketNotaCredito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlE-ticketNotaDebito"){Modulo_CFE_ParametrosGenerales::seturlEticketNotaDebito(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlImixProduccion"){Modulo_CFE_ParametrosGenerales::seturlImixProduccion(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlImixTesting"){Modulo_CFE_ParametrosGenerales::seturlImixTesting(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlWS"){Modulo_CFE_ParametrosGenerales::seturlWS(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="username"){Modulo_CFE_ParametrosGenerales::setusername(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="modoFuncionCFE"){Modulo_CFE_ParametrosGenerales::setmodoFuncionCFE(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="envioArticuloClienteGenerico"){Modulo_CFE_ParametrosGenerales::setenvioArticuloClienteGenerico(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="claveCifrada"){Modulo_CFE_ParametrosGenerales::setclaveCifrada(q.value(rec.indexOf("valorParametro")).toString());}
                else if(q.value(rec.indexOf("nombreParametro")).toString()=="urlDGI"){Modulo_CFE_ParametrosGenerales::seturlDGI(q.value(rec.indexOf("valorParametro")).toString());}



            }
        }
    }else{
        Logs::loguear("Voyager >> ERROR: No se pudo ejecutar la consulta: Modulo_CFE_ParametrosGenerales::cargar");
        Logs::loguear(q.lastError().text());
    }
}


int Modulo_CFE_ParametrosGenerales::rowCount(const QModelIndex & parent) const {
    return m_generico.count();
}

QVariant Modulo_CFE_ParametrosGenerales::data(const QModelIndex & index, int role) const {

    if (index.row() < 0 || index.row() > m_generico.count()){
        return QVariant();

    }

    const CFE_ParametrosGenerales &variable = m_generico[index.row()];

    if (role == nombreParametroRole){
        return variable.nombreParametro();

    }
    else if (role == valorParametroRole){
        return variable.valorParametro();

    }

    return QVariant();
}


QString Modulo_CFE_ParametrosGenerales::retornaValor(QString _codigoConfiguracion) const{



    if(_codigoConfiguracion=="modoTrabajoCFE"){return Modulo_CFE_ParametrosGenerales::getmodoTrabajoCFE();}
    else if(_codigoConfiguracion=="password"){return Modulo_CFE_ParametrosGenerales::getpassword();}
    else if(_codigoConfiguracion=="token"){return Modulo_CFE_ParametrosGenerales::gettoken();}
    else if(_codigoConfiguracion=="urlE-facturaContado"){return Modulo_CFE_ParametrosGenerales::geturlEfacturaContado();}
    else if(_codigoConfiguracion=="urlE-facturaCredito"){return Modulo_CFE_ParametrosGenerales::geturlEfacturaCredito();}
    else if(_codigoConfiguracion=="urlE-facturaNotaCredito"){return Modulo_CFE_ParametrosGenerales::geturlEfacturaNotaCredito();}
    else if(_codigoConfiguracion=="urlE-facturaNotaDebito"){return Modulo_CFE_ParametrosGenerales::geturlEfacturaNotaDebito();}
    else if(_codigoConfiguracion=="urlE-ticketContado"){return Modulo_CFE_ParametrosGenerales::geturlEticketContado();}
    else if(_codigoConfiguracion=="urlE-ticketCredito"){return Modulo_CFE_ParametrosGenerales::geturlEticketCredito();}
    else if(_codigoConfiguracion=="urlE-ticketNotaCredito"){return Modulo_CFE_ParametrosGenerales::geturlEticketNotaCredito();}
    else if(_codigoConfiguracion=="urlE-ticketNotaDebito"){return Modulo_CFE_ParametrosGenerales::geturlEticketNotaDebito();}
    else if(_codigoConfiguracion=="urlImixProduccion"){return Modulo_CFE_ParametrosGenerales::geturlImixProduccion();}
    else if(_codigoConfiguracion=="urlImixTesting"){return Modulo_CFE_ParametrosGenerales::geturlImixTesting();}
    else if(_codigoConfiguracion=="urlWS"){return Modulo_CFE_ParametrosGenerales::geturlWS();}
    else if(_codigoConfiguracion=="username"){return Modulo_CFE_ParametrosGenerales::getusername();}
    else if(_codigoConfiguracion=="modoFuncionCFE"){return Modulo_CFE_ParametrosGenerales::getmodoFuncionCFE();}
    else if(_codigoConfiguracion=="envioArticuloClienteGenerico"){return Modulo_CFE_ParametrosGenerales::getenvioArticuloClienteGenerico();}
    else if(_codigoConfiguracion=="claveCifrada"){return Modulo_CFE_ParametrosGenerales::getclaveCifrada();}
    else if(_codigoConfiguracion=="urlDGI"){return Modulo_CFE_ParametrosGenerales::geturlDGI();}


}



