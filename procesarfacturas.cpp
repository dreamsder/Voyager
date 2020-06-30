#include "procesarfacturas.h"
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
/*#include <QPrinter>
#include <QPaintDevice>
#include <QPainter>
#include <QFont>
#include <QPrinterInfo>*/

#include <curl/curl.h>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <iostream>

#include <QXmlStreamReader>

#include <json/json.h>



#include <CFE/modulo_cfe_parametrosgenerales.h>

Modulo_CFE_ParametrosGenerales func_CFE_ParametrosGenerales;



QString _codigoDocumento;
QString _serieDocumento;
QString _cotizacionMoneda;
QString _codigoLiquidacion;
QString _codigoVendedorLiquidacion;

double doc_precioTotalVenta;
double doc_precioSubTotalVenta;
double doc_precioIvaVenta;

double doc_totalIva1;
double doc_totalIva2;
double doc_totalIva3;

bool emitirDocumentoEnImpresora(QString );

//QRectF cuadro(double x, double y, double ancho, double alto, bool justifica);
//QRectF cuadroA4(double x, double y, double ancho, double alto, bool justifica);
double centimetro;


bool procesarImix(QString, QString);

QString crearJsonIMIX(QString, QString);

bool actualizarNumeroCFEDocumento(QString ,QString , QString );

bool actualizoEstadoDocumentoCFE(QString ,QString , QString );





QString numeroDocumentoV="";
QString codigoTipoDocumentoV="";

QString tipoDeCFEAEnviarDynamiaV="";

QString caeTipoDocumentoCFEDescripcionV="";


#define CURL_ICONV_CODESET_FOR_UTF8 "UTF-8"

QString resultadoFinal="";



ProcesarFacturas::ProcesarFacturas(){}




static int writer(char *data, size_t size, size_t nmemb, std::string *buffer_in)
{

    if (buffer_in != NULL)
    {
        QString resultado = ((QString)data).trimmed();

        if(resultado.contains("<title>Request Error</title>",Qt::CaseInsensitive)){

            Logs::loguear("Respuesta Imix:\n"+resultado);

            return -1;

        }

        QXmlStreamReader xs(resultado);

        while (!xs.atEnd()) {
            if (xs.readNextStartElement()){
                if(xs.name().toString().toLower()!="int"){
                    Logs::loguear("Respuesta Imix:\n"+resultado);

                    return -1;
                }else{
                    QString numeroCae= xs.readElementText();

                    if(actualizarNumeroCFEDocumento(numeroDocumentoV,codigoTipoDocumentoV,numeroCae)){
                        Logs::loguear("Respuesta Imix:\nNúmero CFE OK: "+numeroCae);

                        return size * nmemb;
                    }else{
                        Logs::loguear("ERROR: \nEl CFE se emitio, pero no se pudo actualizar el documento en Khitomer.");
                        return -1;
                    }

                }
            }
        }

        if(resultado.contains("status",Qt::CaseSensitive)){
            resultadoFinal=resultado;
        }
        return size * nmemb;
    }

}


/// Carga las facturas desde la Web hacia Khitomer
bool ProcesarFacturas::cargaDeFacturas(){

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));

    if(query.exec("SELECT FAC.id_factura,FAC.tipo_envio,FAC.fecha_hora,FAC.usu_id, FAC.observacion, FAC.fecha_entrega, FAC.agencia, FAC.depto,FAC.cuidad, FAC.direccion,FAC.nombre,FAC.apellido,FAC.tel ,USU.id_dw FROM factura FAC  join usuarios USU on USU.email=FAC.usu_id where  (FAC.id_factura_dw is null or FAC.id_factura_dw='')  and (USU.id_dw is not null and USU.id_dw!=0)")){
        if(query.first()){
            query.previous();
            while (query.next()) {

                //Sereo los valores para un nuevo documento
                doc_precioTotalVenta=0.00;
                doc_precioSubTotalVenta=0.00;
                doc_precioIvaVenta=0.00;

                doc_totalIva1=0.00;
                doc_totalIva2=0.00;
                doc_totalIva3=0.00;

                //Procedo a guardar las facturas como pendientes
                if(ProcesarFacturas::guardarFactura(query.value(0).toString(),query.value(2).toString(), query.value(13).toString(),query.value(4).toString(),query.value(6).toString(),query.value(7).toString(),query.value(8).toString(),query.value(9).toString(), query.value(10).toString(),query.value(11).toString(),query.value(12).toString())){

                    if(ProcesarFacturas::guardarLineaDocumento(query.value(0).toString())){

                        if(ProcesarFacturas::actualizarDatosDocumento(_codigoDocumento)){

                            if(ProcesarFacturas::actualizarDatoFacturaEnLaWeb(query.value(0).toString(),_codigoDocumento)){


                                if(ProcesarFacturas::retornaValorConfiguracion("MODO_CFE")=="1" && func_CFE_ParametrosGenerales.retornaValor("modoFuncionCFE")=="0"){


                                    if(emitirDocumentoCFEImix(_codigoDocumento,"2","Pendiente")){

                                        if(actualizarEstadoDocumentoAEmitido(_codigoDocumento)){
                                            Logs::loguear("Finalizo el envio a imix del documento '"+_codigoDocumento+"' CORRECTAMENTE");
                                            return true;
                                        }else{
                                            Logs::loguear("CUIDADO: Aunque el documento se envio a Imix, numero '"+_codigoDocumento+"' quedó como pendiente");
                                            return true;
                                        }

                                    }else{
                                        Logs::loguear("ERORR: Fallo el envio a Imix del documento '"+_codigoDocumento+"'");
                                        return false;
                                    }


                                }else{


                                    if(emitirDocumentoEnImpresora(_codigoDocumento)){

                                        if(actualizarEstadoDocumentoAEmitido(_codigoDocumento)){
                                            Logs::loguear("Finalizo la impresion del documento '"+_codigoDocumento+"' CORRECTAMENTE");
                                            return true;
                                        }else{
                                            Logs::loguear("CUIDADO: Aunque la impresión finalizo correctamente, el documento '"+_codigoDocumento+"' quedó como pendiente");
                                            return true;
                                        }
                                    }else{
                                        Logs::loguear("ERORR: Fallo la impresion del documento '"+_codigoDocumento+"'");
                                        return false;
                                    }


                                }



                            }else{
                                Logs::loguear("No se puedo actualizar el numero de documento en la web: "+_codigoDocumento+ " - factura: "+query.value(0).toString());
                                return false;
                            }
                        }else{
                            Logs::loguear("No se puedieron actualizar los datos del documento: "+_codigoDocumento+ " - factura: "+query.value(0).toString());
                            return false;
                        }
                    }else{
                        Logs::loguear("No se puedieron guardar lineas de la factura: "+query.value(0).toString());
                        return false;
                    }
                }else{
                    Logs::loguear("No se puedo guardar la factura: "+query.value(0).toString());
                    return false;
                }
                return true;
            }
            return true;
        }else{
            Logs::loguear("No existen facturas para cargar.");
            return true;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta de carga de facturas:  SELECT * FROM factura...");
        Logs::loguear(query.lastError().text());
        return false;
    }
}


bool ProcesarFacturas::guardarFactura(QString _id_factura,QString _fechaHora,QString _codigoCliente,QString _observacionWeb,QString _agencia,QString _departamento,QString _ciudad,QString _direccion,QString _nombre,QString _apellido,QString _tel) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }

    QString _direccionDeFacturaWeb=_direccion+" -"+_ciudad+" -"+_agencia;

    if(_departamento.toLower().trimmed().contains("montevideo",Qt::CaseInsensitive)){
        if(_ciudad.toLower().trimmed().contains("montevideo",Qt::CaseInsensitive)){
            _direccionDeFacturaWeb=_direccion+" -"+_agencia;
        }
    }else{
        _direccionDeFacturaWeb=_direccion+" -"+_ciudad+" -"+_agencia+" -"+_departamento;
    }


    QSqlQuery query(Database::connect("local"));

    if(query.exec("select codigoDocumento from Documentos where codigoFacturaWeb='"+_id_factura+"' and codigoTipoDocumento='2' and codigoEstadoDocumento='P'")) {
        if(query.first()){

            return false;

        }else{
            query.clear();

            _codigoDocumento=QString::number(ProcesarFacturas::retornoCodigoUltimoDocumentoDisponible());
            _serieDocumento=ProcesarFacturas::retornoSerieDelDocumento();
            _cotizacionMoneda=QString::number(ProcesarFacturas::retornoCotizacionMoneda());
            _codigoLiquidacion=ProcesarFacturas::retornoUltimaLiquidacionActiva();
            _codigoVendedorLiquidacion=ProcesarFacturas::retornoVendedorDeLiquidacion(_codigoLiquidacion);

            if(query.exec("insert INTO Documentos (codigoDocumento, codigoTipoDocumento, serieDocumento, codigoEstadoDocumento, codigoCliente, tipoCliente, codigoMonedaDocumento, fechaEmisionDocumento, precioTotalVenta,precioSubTotalVenta, precioIvaVenta, codigoLiquidacion, codigoVendedorComisiona, usuarioAlta,codigoVendedorLiquidacion,totalIva1,totalIva2,totalIva3,cotizacionMoneda,observaciones,numeroCuentaBancaria,codigoBanco,observacionesWeb,codigoFacturaWeb,esDocumentoWeb,direccionWeb,esDocumentoCFE) "
                          "values('"+_codigoDocumento+"','2','"+_serieDocumento+"','P','"+_codigoCliente+"','1','2','"+QDate::currentDate().toString("yyyy-MM-dd")+"','0','0','0','"+_codigoLiquidacion+"','GISEEL','admin','"+_codigoVendedorLiquidacion+"','0','0','0','"+_cotizacionMoneda+"','','','','"+_observacionWeb+"','"+_id_factura+"','1','"+_direccionDeFacturaWeb+"','1')")){

                return true;
            }else{
                Logs::loguear("ERROR: No se pudo insertar el documento: "+_codigoDocumento+ " - factura: "+_id_factura);
                return false;
            }
        }
    }else{
        return false;
    }
}


qlonglong ProcesarFacturas::retornoCodigoUltimoDocumentoDisponible() {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return 1;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoDocumento FROM Documentos where codigoTipoDocumento='2' order by codigoDocumento desc limit 1")) {
        if(query.first()){
            if(query.value(0).toString()!=0){
                return query.value(0).toLongLong()+1;
            }else{
                return 1;
            }
        }else{return 1;}
    }else{
        return 1;
    }
}


QString ProcesarFacturas::retornoSerieDelDocumento() {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "A";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT serieDocumento FROM TipoDocumento where codigoTipoDocumento=2 limit 1")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toString();
            }else{
                return "A";
            }
        }else{return "A";}
    }else{
        return "A";
    }
}

double ProcesarFacturas::retornoCotizacionMoneda() {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT cotizacionMoneda FROM Monedas where codigoMoneda=2")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toDouble();
            }else{
                return 0.00;
            }
        }else{return 0.00;}
    }else{
        return 0.00;
    }
}

QString ProcesarFacturas::retornoUltimaLiquidacionActiva() {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoLiquidacion FROM Liquidaciones where estadoLiquidacion='A' order by fechaLiquidacion desc limit 1")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toString();
            }else{
                return "";
            }
        }else{return "";}
    }else{
        return "";
    }
}


QString ProcesarFacturas::retornoVendedorDeLiquidacion(QString _codigoLiquidacion) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoVendedor FROM Liquidaciones where codigoLiquidacion='"+_codigoLiquidacion+"' and estadoLiquidacion='A' order by fechaLiquidacion desc limit 1")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toString();
            }else{
                return "";
            }
        }else{return "";}
    }else{
        return "";
    }
}








bool ProcesarFacturas::guardarLineaDocumento(QString _codigoFactura) {


    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }



    QSqlQuery query(Database::connect("remota"));
    QSqlQuery queryInsertLocal(Database::connect("local"));

    int i=0;
    if(query.exec("SELECT prod_id,qty,precio_unitario FROM factura_productos where id_factura='"+_codigoFactura+"'")){
        if(query.first()){
            query.previous();
            while (query.next()){

                 int codigoIva=ProcesarFacturas::retornoCodigoIva(query.value(0).toString());
                //Analizo las cantidades y grabo siempre cantidad 1
                for (int var = 0; var < query.value(1).toInt(); var++) {

                    double _precioTotalVenta = query.value(2).toDouble();//*query.value(1).toDouble();
                    doc_precioTotalVenta+=_precioTotalVenta;

                    double _precioIvaArticulo= (_precioTotalVenta)-(_precioTotalVenta/ProcesarFacturas::retornoFactorMultiplicador(query.value(0).toString()));
                    doc_precioIvaVenta+=_precioIvaArticulo;
                    doc_precioSubTotalVenta+=_precioTotalVenta-_precioIvaArticulo;


                    if(codigoIva==1){
                        doc_totalIva1+=_precioIvaArticulo;
                    }else if(codigoIva==2){
                        doc_totalIva2+=_precioIvaArticulo;
                    }else if(codigoIva==3){
                        doc_totalIva3+=_precioIvaArticulo;
                    }

                    queryInsertLocal.clear();
                    queryInsertLocal.exec("insert INTO DocumentosLineas (codigoDocumento, codigoTipoDocumento, numeroLinea, codigoArticulo, codigoArticuloBarras, cantidad, precioTotalVenta, precioArticuloUnitario, precioIvaArticulo)"
                                          "values('"+_codigoDocumento+"','2','"+QString::number(i)+"','"+query.value(0).toString()+"','','1','"+QString::number(_precioTotalVenta,'f',2)+"','"+query.value(2).toString()+"','"+QString::number(_precioIvaArticulo,'f',2)+"')");

                    i++;


                }
            }
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}








double ProcesarFacturas::retornoFactorMultiplicador(QString _codigoArticulo) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return 1.00;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT IV.factorMultiplicador FROM Articulos AR join Ivas IV on IV.codigoIva=AR.codigoIva where AR.codigoArticulo='"+_codigoArticulo+"'")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toDouble();
            }else{
                return 1.00;
            }
        }else{return 1.00;}
    }else{
        return 1.00;
    }
}

int ProcesarFacturas::retornoCodigoIva(QString _codigoArticulo) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return 1;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("SELECT codigoIva FROM Articulos  where codigoArticulo='"+_codigoArticulo+"'")) {
        if(query.first()){
            if(query.value(0).toString()!=""){
                return query.value(0).toInt();
            }else{
                return 1;
            }
        }else{return 1;}
    }else{
        return 1;
    }
}

bool ProcesarFacturas::actualizarDatosDocumento(QString _codigoDocumentoX) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("UPDATE Documentos set precioTotalVenta='"+QString::number(doc_precioTotalVenta,'f',2)+"', saldoClienteCuentaCorriente='"+QString::number(doc_precioTotalVenta,'f',2)+"', precioSubTotalVenta='"+QString::number(doc_precioSubTotalVenta,'f',2)+"', precioIvaVenta='"+QString::number(doc_precioIvaVenta,'f',2)+"', totalIva1='"+QString::number(doc_totalIva1,'f',2)+"', totalIva2='"+QString::number(doc_totalIva2,'f',2)+"',totalIva3='"+QString::number(doc_totalIva3,'f',2)+"' where codigoDocumento='"+_codigoDocumentoX+"' and codigoTipoDocumento=2  ")) {
        return true;
    }else{
        return false;
    }
}

bool ProcesarFacturas::actualizarEstadoDocumentoAEmitido(QString _codigoDocumentoX) {

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("UPDATE Documentos set codigoEstadoDocumento='E' where codigoDocumento='"+_codigoDocumentoX+"' and codigoTipoDocumento=2  ")) {
        return true;
    }else{

        return false;
    }
}


bool ProcesarFacturas::actualizarDatoFacturaEnLaWeb(QString _codigoFactura,QString _codigoDocumentoX) {

    Database::cehqueStatusAccesoMysql("remota");

    if(!Database::connect("remota").isOpen()){
        if(!Database::connect("remota").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseRemoto());
            return false;
        }
    }

    QSqlQuery query(Database::connect("remota"));

    if(query.exec("UPDATE factura set id_factura_dw='"+_codigoDocumentoX+"' where id_factura='"+_codigoFactura+"' ")) {
        return true;
    }else{
        return false;
    }
}


















/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**                                            Esta parte hacia abajo son todos los metodos de impresión                                                    **/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// ##########################################################################################################################
/// ######## Realiza los calculos para posicionar los campos del modelo de impresion e imprime el documento ##################
/// ##########################################################################################################################


bool emitirDocumentoEnImpresora(QString _codigoDocumentoDOC){
    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }


    QString _codigoModeloImpresion="";
    double altoPagina;
    double anchoPagina;
    double comienzoCuerpo;
    double comienzoPie;
    double cantidadItemsPorHoja=0;
    int fuenteSizePoints=8;

    double cantidadLineas=0;


    QSqlQuery query(Database::connect("local"));


    if(query.exec("SELECT codigoModeloImpresion FROM TipoDocumento where codigoTipoDocumento='2' ")) {
        if(query.first()){
            if(query.value(0).toString()!=0){
                _codigoModeloImpresion = query.value(0).toString();
            }else{
                Logs::loguear("ERROR: El documento factura credito(2) no tiene un modelo de impresión asignado");
                return false;
            }
        }else{
            Logs::loguear("ERROR: No de encontro el tipo de documento factura credito(2), para comenzar a imprimir");
            return false;
        }
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
        Logs::loguear(query.lastQuery());
        Logs::loguear(query.lastError().text());
        return false;
    }



    if(_codigoModeloImpresion=="")
        return false;


    query.clear();

    if(query.exec("SELECT altoPagina,anchoPagina,comienzoCuerpo,comienzoPie,fuenteSizePoints FROM ModeloImpresion where codigoModeloImpresion='"+_codigoModeloImpresion+"'")) {
        if(query.first()){
            if(query.value(0).toString()!=0){
                altoPagina = query.value(0).toDouble()*10;
            }else{
                return false;
            }

            if(query.value(1).toString()!=0){
                anchoPagina = query.value(1).toDouble()*10;
            }else{
                return false;
            }

            if(query.value(2).toString()!=0){
                comienzoCuerpo = query.value(2).toDouble();
            }else{
                return false;
            }

            if(query.value(3).toString()!=0){
                comienzoPie = query.value(3).toDouble();
            }else{
                return false;
            }

            if(query.value(4).toString()!=0){
                fuenteSizePoints = query.value(4).toInt();
            }else{
                return false;
            }
        }else{return false;}
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
        Logs::loguear(query.lastQuery());
        Logs::loguear(query.lastError().text());
        return false;
    }
    int MaximoCopias=1;


    double seteos=comienzoCuerpo;


    query.clear();


    if(query.exec("SELECT cantidadCopias FROM TipoDocumento where codigoTipoDocumento='2'")) {
        if(query.first()){
            if(query.value(0).toString()!=0){
                MaximoCopias = query.value(0).toInt();
            }
        }else{return false;}
    }else{
        Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
        Logs::loguear(query.lastQuery());
        Logs::loguear(query.lastError().text());
        return false;
    }

    query.clear();

    /// Obtengo la cantidad de lineas del cuerpo a imprimir
    if(query.exec("SELECT count(distinct DOC.codigoArticulo) FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2'")) {
        if(query.first()){
            if(query.value(0).toString()!=0){
                cantidadLineas = query.value(0).toInt();
            }else{
                cantidadLineas = 0;
            }
        }else{
            cantidadLineas = 0;}
    }


    ///Obtengo la cantidad de lineas que puedo usar en la hoja
    cantidadItemsPorHoja= ((comienzoPie-comienzoCuerpo)*26)/13;

    int MaximoHojasAImprimir=0;


    ///Calculo la cantidad de hojas necesarias para imprimir la factura
    if(cantidadLineas<=cantidadItemsPorHoja){

        MaximoHojasAImprimir=1;

    }else{

        if((cantidadLineas/cantidadItemsPorHoja)<=2){
            MaximoHojasAImprimir=2;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=3){
            MaximoHojasAImprimir=3;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=4){
            MaximoHojasAImprimir=4;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=5){
            MaximoHojasAImprimir=5;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=6){
            MaximoHojasAImprimir=6;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=7){
            MaximoHojasAImprimir=7;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=8){
            MaximoHojasAImprimir=8;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=9){
            MaximoHojasAImprimir=9;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=10){
            MaximoHojasAImprimir=10;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=11){
            MaximoHojasAImprimir=11;
        }else if((cantidadLineas/cantidadItemsPorHoja)<=12){
            MaximoHojasAImprimir=12;
        }

    }

    //##################################################
    // Preparo los seteos de la impresora ##############
/*    QPrinter printer;
    printer.setPrinterName(QPrinterInfo::defaultPrinter().printerName());
    QPainter painter;
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setPageSize(QPrinter::A4);
    QSizeF size;
    size.setHeight(altoPagina);
    size.setWidth(anchoPagina);
    //printer.setPaperSize(size,QPrinter::Millimeter);
    printer.setOrientation(QPrinter::Portrait);
    printer.setCreator("Khitomer");
    printer.setColorMode(QPrinter::GrayScale);
    printer.setFullPage(true);
    QFont fuente("Arial");
    fuente.setPointSize(fuenteSizePoints);


    centimetro = printer.QPaintDevice::width()/(printer.QPaintDevice::widthMM()/10);

    // FIN de preparo los seteos de la impresora ######
    //#################################################

    QSqlQuery queryCabezal(Database::connect("local"));
    QSqlQuery queryCuerpo(Database::connect("local"));
    QSqlQuery queryPie(Database::connect("local"));



    int cantidadVecesIncremento=cantidadItemsPorHoja;


    //Cantidad de veces que se repite la impresion del mismo documento
    for(int i=0;i<MaximoCopias;i++){

        int contadorLineas=0;
        int iterador=0;
        comienzoCuerpo=seteos;
        cantidadItemsPorHoja=cantidadVecesIncremento;



        if (! painter.begin(&printer)) {
            return false;
        }
        painter.setFont(fuente);


        // Cantidad de hojas que se imprimen del mismo documento
        for(int j=0;j<MaximoHojasAImprimir;j++){



            if(j<MaximoHojasAImprimir && j!=0){

                printer.newPage();

                iterador+=cantidadVecesIncremento;
                cantidadItemsPorHoja+=cantidadVecesIncremento;
                comienzoCuerpo=seteos;

            }


            ///Armamos el cabezal de la impresion

            if(query.exec("select MIL.posicionX,MIL.posicionY,MIL.largoDeCampo,MIL.alineacion,case when MIL.alineacion=2 then true else false end,IC.campoEnTabla,IC.codigoEtiqueta from ModeloImpresionLineas MIL join ImpresionCampos IC on IC.codigoCampoImpresion=MIL.codigoCampoImpresion where MIL.codigoModeloImpresion='"+_codigoModeloImpresion+"' and IC.tipoCampo=1")) {

                while(query.next()){
                    queryCabezal.clear();
                    if(queryCabezal.exec("SELECT case when "+query.value(5).toString()+"='' then ' ' else "+query.value(5).toString()+"  end, Doc.codigoCliente,Doc.tipoCliente FROM Documentos Doc left join Clientes C on C.codigoCliente=Doc.codigoCliente and C.tipoCliente=Doc.tipoCliente  join TipoDocumento TD on TD.codigoTipoDocumento=Doc.codigoTipoDocumento  where Doc.codigoDocumento='"+_codigoDocumentoDOC+"' and Doc.codigoTipoDocumento='2'")) {
                        queryCabezal.next();
                        if(queryCabezal.value(0).toString()!=0){

                            if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                if(query.value(5).toString()=="Doc.fechaEmisionDocumento"){
                                    painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QDateTime::fromString(queryCabezal.value(0).toString(),Qt::ISODate).toString("dd-MM-yyyy"));
                                }else{
                                    if(query.value(6).toString()=="txtMarcaXDeClienteFinalCampo"){
                                        //chequeo si el cliente tiene RUT, si no tiene, imprimo la X de cliente final
                                        if(!ProcesarFacturas::retornaClienteTieneRUT(queryCabezal.value(1).toString().trimmed(),queryCabezal.value(2).toString().trimmed())){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),"X");
                                        }
                                    }else if(query.value(6).toString()=="txtDireccionCampo"){

                                        /// Chequeo si tengo una direccion de la web para imprimir, si la tengo la imprimo,
                                        /// sino, imprimo la direccion por defecto
                                        QString _direccionWeb= ProcesarFacturas::retornaDireccionWebFactura(_codigoDocumentoDOC,"2");

                                        if(_direccionWeb!=""){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),_direccionWeb);
                                        }else{
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCabezal.value(0).toString());
                                        }

                                    }else{
                                        painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCabezal.value(0).toString());
                                    }
                                }
                            }

                            if(query.value(5).toString()=="Doc.fechaEmisionDocumento"){
                                painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QDateTime::fromString(queryCabezal.value(0).toString(),Qt::ISODate).toString("dd-MM-yyyy"));
                            }else{
                                if(query.value(6).toString()=="txtMarcaXDeClienteFinalCampo"){

                                    //chequeo si el cliente tiene RUT, si no tiene, imprimo la X de cliente final
                                    if(!ProcesarFacturas::retornaClienteTieneRUT(queryCabezal.value(1).toString().trimmed(),queryCabezal.value(2).toString().trimmed())){

                                        painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),"X");

                                    }
                                }else if(query.value(6).toString()=="txtDireccionCampo"){

                                    /// Chequeo si tengo una direccion de la web para imprimir, si la tengo la imprimo,
                                    /// sino, imprimo la direccion por defecto
                                    QString _direccionWeb= ProcesarFacturas::retornaDireccionWebFactura(_codigoDocumentoDOC,"2");

                                    if(_direccionWeb!=""){
                                        painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),_direccionWeb);
                                    }else{
                                        painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCabezal.value(0).toString());
                                    }

                                }else{
                                    painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCabezal.value(0).toString());
                                }
                            }
                        }
                    }
                }
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
                Logs::loguear(query.lastQuery());
                Logs::loguear(query.lastError().text());
                return false;
            }







            ///Armamos el cuerpo de la impresion
            query.clear();
            if(query.exec("select MIL.posicionX,MIL.posicionY,MIL.largoDeCampo,MIL.alineacion,case when MIL.alineacion=2 then true else false end,IC.campoEnTabla from ModeloImpresionLineas MIL join ImpresionCampos IC on IC.codigoCampoImpresion=MIL.codigoCampoImpresion where MIL.codigoModeloImpresion='"+_codigoModeloImpresion+"' and IC.tipoCampo=2")) {

                while(query.next()){
                    comienzoCuerpo=seteos;

                    queryCuerpo.clear();
                    if(query.value(5).toString()=="CANTIDAD"){

                        if(queryCuerpo.exec("SELECT DOC.codigoArticulo, sum(DOC.cantidad)  FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2' group by DOC.codigoArticulo")) {

                            contadorLineas=0;
                            while(queryCuerpo.next()){


                                if(queryCuerpo.value(0).toString()!=0){

                                    if(contadorLineas<cantidadItemsPorHoja && contadorLineas >=iterador ){

                                        if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCuerpo.value(1).toString());
                                        }
                                        painter.drawText(cuadro(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCuerpo.value(1).toString());

                                        comienzoCuerpo+=0.5;

                                    }
                                    contadorLineas++;
                                }

                            }
                        }

                    }else if(query.value(5).toString()=="precioTotalVenta"){
                        if(queryCuerpo.exec("SELECT DOC.codigoArticulo, sum(precioTotalVenta)  FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2' group by DOC.codigoArticulo")) {

                            contadorLineas=0;

                            while(queryCuerpo.next()){
                                if(queryCuerpo.value(0).toString()!=0){

                                    if(contadorLineas<cantidadItemsPorHoja && contadorLineas >=iterador){

                                        if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QString::number(queryCuerpo.value(1).toDouble(),'f',2));
                                        }
                                        painter.drawText(cuadro(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QString::number(queryCuerpo.value(1).toDouble(),'f',2));
                                        comienzoCuerpo+=0.5;

                                    }
                                    contadorLineas++;
                                }
                            }
                        }
                    }else if(query.value(5).toString()=="precioArticuloUnitario"){
                        if(queryCuerpo.exec("SELECT DOC.codigoArticulo, precioArticuloUnitario  FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2' group by DOC.codigoArticulo")) {

                            contadorLineas=0;

                            while(queryCuerpo.next()){
                                if(queryCuerpo.value(0).toString()!=0){

                                    if(contadorLineas<cantidadItemsPorHoja && contadorLineas >=iterador){
                                        if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QString::number(queryCuerpo.value(1).toDouble(),'f',2));
                                        }
                                        painter.drawText(cuadro(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),QString::number(queryCuerpo.value(1).toDouble(),'f',2));
                                        comienzoCuerpo+=0.5;

                                    }
                                    contadorLineas++;
                                }
                            }
                        }
                    }else if(query.value(5).toString()=="descripcionArticulo"){
                        if(queryCuerpo.exec("SELECT DOC.codigoArticulo  FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2' group by DOC.codigoArticulo")) {

                            contadorLineas=0;

                            while(queryCuerpo.next()){
                                if(queryCuerpo.value(0).toString()!=0){

                                    if(contadorLineas<cantidadItemsPorHoja && contadorLineas >=iterador){

                                        if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                            painter.drawText(cuadroA4(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(), ProcesarFacturas::retornaDescripcionArticulo(queryCuerpo.value(0).toString()));
                                        }
                                        painter.drawText(cuadro(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(), ProcesarFacturas::retornaDescripcionArticulo(queryCuerpo.value(0).toString()));
                                        comienzoCuerpo+=0.5;

                                    }
                                    contadorLineas++;
                                }
                            }
                        }
                    }else{
                        if(queryCuerpo.exec("SELECT DOC.codigoArticulo, case when "+query.value(5).toString()+"='' then ' ' else "+query.value(5).toString()+"  end  FROM DocumentosLineas DOC where DOC.codigoDocumento='"+_codigoDocumentoDOC+"' and DOC.codigoTipoDocumento='2' group by DOC.codigoArticulo")) {

                            contadorLineas=0;

                            while(queryCuerpo.next()){
                                if(queryCuerpo.value(0).toString()!=0){

                                    if(contadorLineas<cantidadItemsPorHoja && contadorLineas >=iterador){
                                        if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){

                                            painter.drawText(cuadroA4(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCuerpo.value(1).toString());
                                        }
                                        painter.drawText(cuadro(query.value(0).toDouble(),comienzoCuerpo,query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryCuerpo.value(1).toString());
                                        comienzoCuerpo+=0.5;

                                    }
                                    contadorLineas++;
                                }
                            }
                        }
                    }


                }
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
                Logs::loguear(query.lastQuery());
                Logs::loguear(query.lastError().text());
                return false;
            }








            ///Armamos el pie de la impresion
            query.clear();
            if(query.exec("select MIL.posicionX,MIL.posicionY,MIL.largoDeCampo,MIL.alineacion,case when MIL.alineacion=2 then true else false end,IC.campoEnTabla from ModeloImpresionLineas MIL join ImpresionCampos IC on IC.codigoCampoImpresion=MIL.codigoCampoImpresion where MIL.codigoModeloImpresion='"+_codigoModeloImpresion+"' and IC.tipoCampo=3")) {
                while(query.next()){
                    queryPie.clear();
                    if(queryPie.exec("SELECT case when "+query.value(5).toString()+"='' then ' ' else "+query.value(5).toString()+"  end FROM Documentos Doc left join Clientes C on C.codigoCliente=Doc.codigoCliente and C.tipoCliente=Doc.tipoCliente join Monedas on Monedas.codigoMoneda=Doc.codigoMonedaDocumento where Doc.codigoDocumento='"+_codigoDocumentoDOC+"' and Doc.codigoTipoDocumento='2'")) {
                        queryPie.next();
                        if(queryPie.value(0).toString()!=0){

                            if(ProcesarFacturas::retornaValorConfiguracion("MODO_IMPRESION_A4")=="1"){
                                painter.drawText(cuadroA4(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryPie.value(0).toString());
                            }
                            painter.drawText(cuadro(query.value(0).toDouble(),query.value(1).toDouble(),query.value(2).toDouble(),1,query.value(4).toBool()),query.value(3).toInt(),queryPie.value(0).toString());
                        }
                    }
                }
            }else{
                Logs::loguear("ERROR: No se pudo ejecutar la consulta:");
                Logs::loguear(query.lastQuery());
                Logs::loguear(query.lastError().text());
                return false;
            }

        }
        painter.end();
    }
*/
    return true;
}






//################################################################################
//######## Calcula en centimetros la posicion de los campos a imprimirse #########
//################################################################################
/*QRectF cuadro(double x, double y, double ancho, double alto,bool justifica=false){

    QRectF punto(x*centimetro,y*centimetro,ancho*centimetro,alto*centimetro);
    if(justifica){

        if(x>ancho)
            punto=QRect((x*centimetro)-(ancho*centimetro),y*centimetro,ancho*centimetro,alto*centimetro);

    }
    return punto;
}
QRectF cuadroA4(double x, double y, double ancho, double alto,bool justifica=false){

    QRectF punto(x*centimetro,(y+15)*centimetro,ancho*centimetro,alto*centimetro);
    if(justifica){

        if(x>ancho)
            punto=QRect((x*centimetro)-(ancho*centimetro),(y+15)*centimetro,ancho*centimetro,alto*centimetro);

    }
    return punto;
}*/


QString ProcesarFacturas::retornaValorConfiguracion(QString _codigoConfiguracion) {
    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "-1";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("select valorConfiguracion from Configuracion where codigoConfiguracion='"+_codigoConfiguracion+"'")) {

        if(query.first()){

            if(query.value(0).toString()!=""){

                return query.value(0).toString();

            }else{
                return "-1";
            }
        }else{
            return "-1";
        }
    }else{
        return "-1";
    }
}

bool ProcesarFacturas::retornaClienteTieneRUT(QString _codigoCliente,QString _codigoTipoCliente) {
    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("select rut from Clientes where codigoCliente='"+_codigoCliente+"' and tipoCliente='"+_codigoTipoCliente+"' limit 1")) {
        if(query.first()){
            if(query.value(0).toString()=="" || query.value(0).toString().toLower().trimmed() =="null" || query.value(0).toString().isNull() || query.value(0).toString().isEmpty()){
                return false;
            }else{
                return true;
            }
        }else{return false;}
    }else{
        return false;
    }
}

QString ProcesarFacturas::retornaDescripcionArticulo(QString _codigoArticulo) {
    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("select descripcionArticulo from Articulos where codigoArticulo='"+_codigoArticulo+"'")) {

        if(query.first()){
            if(query.value(0).toString()!=""){

                return query.value(0).toString();

            }else{
                return "";
            }
        }else{return "";}

    }else{
        return "";
    }
}

QString ProcesarFacturas::retornaDireccionWebFactura(QString _codigoDocumento,QString _codigoTipoDocumento) {
    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "";
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("select direccionWeb from Documentos where codigoDocumento='"+_codigoDocumento+"' and codigoTipoDocumento='"+_codigoTipoDocumento+"'")) {
        if(query.first()){
            if(query.value(0).toString()=="" || query.value(0).toString().toLower().trimmed() =="null" || query.value(0).toString().isNull() || query.value(0).toString().isEmpty()){
                return "";
            }else{
                return query.value(0).toString();
            }
        }else{
            return "";
        }
    }else{
        return "";
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**                                            Esta parte hacia abajo son todas las funciones para emitir CFE                                                  **/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool ProcesarFacturas::emitirDocumentoCFEImix(QString _codigoDocumento,QString _codigoTipoDocumento, QString _descripcionEstadoActualDocumento){

    //Consulto si el modo es 0 = imix, 1 = dynamia
    if(func_CFE_ParametrosGenerales.retornaValor("modoFuncionCFE")=="0"){

        bool resultadoprocesarImix= procesarImix(_codigoDocumento, _codigoTipoDocumento);

        //Si se procesa correctamente el cfe retorno true
        if(resultadoprocesarImix){
            return true;
        }else{

            if(_descripcionEstadoActualDocumento=="Pendiente"){

                actualizoEstadoDocumentoCFE(_codigoDocumento, _codigoTipoDocumento,"P");

                return false;
            }else{

                return false;

                //   Si falla, intento eliminar el documento en la base
                /*bool resultado2 = eliminarDocumento(_codigoDocumento, _codigoTipoDocumento);

                if(resultado2){
                    return false;
                }else{
                    Logs::loguear("Voyager >> ERROR: Hubo un error al intentar eliminar la informacion del documento");
                    return false;
                }*/
            }
        }

    }else{
        return false;
    }


}



bool procesarImix(QString _codigoDocumento,QString _codigoTipoDocumento){

    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);

    QString str1 = crearJsonIMIX(_codigoDocumento, _codigoTipoDocumento);

    Logs::loguear("Modo CFE Imix: \n\nEnvio a Imix:\n"+str1+"\n\n");

    if(str1=="-1"){

        Logs::loguear("ERROR: \nNo existe documento para enviar como CFE");
        return false;
    }else if(str1=="-2"){
        Logs::loguear("ERROR: \nNo se puede ejecutar la consulta a la base de datos");

        return false;
    }else if(str1=="-3"){
        Logs::loguear("ERROR: \nNo hay conexión con la base de datos");

        return false;
    }


    numeroDocumentoV=_codigoDocumento;
    codigoTipoDocumentoV=_codigoTipoDocumento;

    QByteArray baddddd = str1.toUtf8();
    const char *mensajeAEnviarPost = baddddd.data();

    struct curl_slist *headers=NULL; // init to NULL is important
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");

    curl = curl_easy_init();
    if(curl) {

        resultadoFinal="";

        QByteArray testing =    func_CFE_ParametrosGenerales.retornaValor("urlImixTesting").toLatin1();
        QByteArray produccion = func_CFE_ParametrosGenerales.retornaValor("urlImixProduccion").toLatin1();


        const char *c_conexion;
        if(func_CFE_ParametrosGenerales.retornaValor("modoTrabajoCFE")=="0"){
            c_conexion=testing.data();
        }else{
            c_conexion=produccion.data();
        }

        curl_easy_setopt(curl, CURLOPT_URL, c_conexion);


        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST,1);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(mensajeAEnviarPost));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mensajeAEnviarPost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);


        res = curl_easy_perform(curl);

        if(res != CURLE_OK){

            QString resultadoError=curl_easy_strerror(res);

            if(resultadoError.trimmed()=="Couldn't resolve host name" || resultadoError.trimmed()=="Couldn't connect to server"){
                Logs::loguear("ERROR: No hay conexión con Imix: \n\n"+(QString)c_conexion);
            }else{
                Logs::loguear("ERROR: "+resultadoError+"\n\nConexión: \n\n"+(QString)c_conexion);
            }

            return false;
        }
        else {
            resultadoFinal =  resultadoFinal.remove(resultadoFinal.length()-1,1);
            return true;

        }
        curl_easy_cleanup(curl);
    }
    else{
        return false;
    }
    curl_global_cleanup();



}


bool actualizarNumeroCFEDocumento(QString _codigoDocumento,QString _codigoTipoDocumento, QString _numeroCae){

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("update Documentos set cae_numeroCae='"+_numeroCae+"' where codigoDocumento='"+_codigoDocumento+"' and codigoTipoDocumento='"+_codigoTipoDocumento+"'")) {
        return true;
    }else{
        return false;
    }

}

bool actualizoEstadoDocumentoCFE(QString _codigoDocumento,QString _codigoTipoDocumento, QString _estadoDocumento) {


    if(_codigoDocumento.trimmed()=="" || _codigoTipoDocumento.trimmed()=="" ){
        return false;
    }


    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return false;
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec("update Documentos set codigoEstadoDocumento='"+_estadoDocumento+"'  where codigoDocumento='"+_codigoDocumento+"' and codigoTipoDocumento='"+_codigoTipoDocumento+"'")) {
        return true;
    }else{
        return false;
    }


}



QString crearJsonIMIX(QString _codigoDocumento,QString _codigoTipoDocumento){

    QString consultaSql = " SELECT ";
    consultaSql +=" TD.descripcionTipoDocumentoCFE'Comprobante', ";
    consultaSql +=" DOC.serieDocumento'Serie', ";
    consultaSql +=" DOC.codigoDocumento'Numero', ";
    consultaSql +=" UNIX_TIMESTAMP(STR_TO_DATE(DOC.fechaEmisionDocumento, '%Y-%m-%d '))'Fecha', ";
    consultaSql +=" case when DOC.tipoCliente=1 then  DOC.codigoCliente else 'null' end 'Cliente', ";
    consultaSql +=" case when DOC.tipoCliente=2 then  DOC.codigoCliente else 'null' end 'Proveedor', ";
    consultaSql +=" left(trim(CLI.razonSocial),32) 'RazonSocial', ";
    consultaSql +=" left(trim(CLI.direccion),32) 'Direccion',     ";

    consultaSql +=" case when CLI.rut is null then 'null' else case when CLI.codigoTipoDocumentoCliente=2 then 'null' else case when CLI.codigoTipoDocumentoCliente in (3,4,5,6) and (CLI.rut is not null and trim(CLI.rut)!='') then  CLI.rut else 'null'end end end 'Documento',";
    consultaSql +=" case when CLI.codigoTipoDocumentoCliente in (4,5,6) then PA.codigoIsoPais else 'null' end 'PaisDocumento',";
    consultaSql +=" case when CLI.codigoTipoDocumentoCliente in (4,5,6) then CLI.codigoTipoDocumentoCliente else 'null' end 'TipoDocumento',";
    consultaSql +=" case when CLI.rut is null then 'null' else case when CLI.codigoTipoDocumentoCliente = 2 then CLI.rut else 'null' end end 'Rut',";


    //consultaSql +=" case when CLI.rut is null then 'null' else CLI.rut end 'Documento', ";
    //consultaSql +=" case when CLI.rut is null then 'null' else case when CLI.codigoTipoDocumentoCliente in (2,3) then 'null' else PA.codigoIsoPais end end 'PaisDocumento', ";
    //consultaSql +=" case when CLI.rut is null then 'null' else case when CLI.codigoTipoDocumentoCliente in (2,3) then 'null' else CFETDC.descripcionTipoDocumentoCliente end end 'TipoDocumento', ";
    //consultaSql +=" case when CLI.rut is null then 'null' else case when CLI.codigoTipoDocumentoCliente = 2 then CLI.rut else 'null' end end 'Rut', ";

    consultaSql +=" MO.codigoISO4217'Moneda', ";
    consultaSql +=" left(DOC.porcentajeDescuentoAlTotal/100,6)'PorcentajeDescuento',     ";
    consultaSql +=" DOC.precioTotalVenta'Total',    ";

    consultaSql +=" DL.codigoArticulo'Articulo', ";
    consultaSql +=" left(trim(AR.descripcionArticulo),32)'Descripcion', ";
    consultaSql +=" DL.cantidad'Cantidad', ";
    consultaSql +=" DL.precioArticuloUnitario'Precio', ";
    consultaSql +=" concat('N° ',DOC.codigoDocumento, ' - ',left(trim(DOC.observaciones),48))'Observaciones'  ";


    consultaSql +=" FROM DocumentosLineas DL      ";
    consultaSql +=" join Documentos DOC on DOC.codigoDocumento=DL.codigoDocumento and DOC.codigoTipoDocumento=DL.codigoTipoDocumento ";
    consultaSql +=" join Articulos AR on AR.codigoArticulo=DL.codigoArticulo ";
    consultaSql +=" join TipoDocumento TD on TD.codigoTipoDocumento=DOC.codigoTipoDocumento ";
    consultaSql +=" join Monedas MO on MO.codigoMoneda = DOC.codigoMonedaDocumento ";
    consultaSql +=" join Clientes CLI on CLI.codigoCliente=DOC.codigoCliente and CLI.tipoCliente=DOC.tipoCliente ";
    consultaSql +=" join CFE_TipoDocumentoCliente CFETDC on CFETDC.codigoTipoDocumentoCliente=CLI.codigoTipoDocumentoCliente ";
    consultaSql +=" join Pais PA on PA.codigoPais=CLI.codigoPais ";
    consultaSql +=" where DOC.codigoDocumento="+_codigoDocumento+" and DOC.codigoTipoDocumento="+_codigoTipoDocumento+" and DOC.esDocumentoCFE='1' ";

    Database::cehqueStatusAccesoMysql("local");

    if(!Database::connect("local").isOpen()){
        if(!Database::connect("local").open()){
            Logs::loguear("ERROR: No hay conexion a la base de datos: "+ConfiguracionXml::getBaseLocal());
            return "-3"; //No hay conexión con la base de datos
        }
    }
    QSqlQuery query(Database::connect("local"));

    if(query.exec(consultaSql)) {

        if(query.first()){


            //Variables cabezal:
            QString Comprobante=query.value(0).toString().replace("\n","");
            //    QString Serie=query.value(1).toString();
            //    QString Numero=query.value(2).toString();
            QString Fecha=query.value(3).toString().replace("\n","");
            QString Cliente=query.value(4).toString().replace("\n","");
            QString Proveedor=query.value(5).toString().replace("\n","");
            QString RazonSocial=query.value(6).toString().toUpper().replace("Á","A").replace("É","E").replace("Í","I").replace("Ó","O").replace("Ú","U").replace("\"","\\\"").replace("\n","");



            QString Direccion=query.value(7).toString().toUpper().replace("Á","A").replace("É","E").replace("Í","I").replace("Ó","O").replace("Ú","U").replace("\"","\\\"").replace("\n","");
            QString Documento=query.value(8).toString().replace("\n","");
            QString PaisDocumento=query.value(9).toString().replace("\n","");
            QString TipoDocumento=query.value(10).toString().replace("\n","");
            QString Rut=query.value(11).toString().replace("\n","");
            QString Moneda=query.value(12).toString().replace("\n","");
            QString PorcentajeDescuento=query.value(13).toString();
            QString Total=query.value(14).toString();
            QString Observacion=query.value(19).toString().toUpper().replace("Á","A").replace("É","E").replace("Í","I").replace("Ó","O").replace("Ú","U").replace("\"","\\\"").replace("\n","");


            QString ClienteOProveedor="Cliente";
            QString DatoDocumento=",\"Documento\":\""+Documento+"\"";
            QString DatoPaisDocumento=",\"PaisDocumento\":\""+PaisDocumento+"\"";
            QString DatoTipoDocumento=",\"TipoDocumento\":"+TipoDocumento+"";
            QString DatoRut= ",\"Rut\":\""+Rut+"\"";

            QString DatoFecha= ",\"Fecha\":\"/Date("+Fecha+")/\"";

            DatoFecha="";

            if(Cliente=="null"){
                Cliente=Proveedor;
                ClienteOProveedor="Proveedor";
            }

            if(func_CFE_ParametrosGenerales.retornaValor("envioArticuloClienteGenerico")=="1"){
                Cliente="1";
            }


            if(Documento=="null")
                DatoDocumento="";

            if(PaisDocumento=="null")
                DatoPaisDocumento="";

            if(TipoDocumento=="null")
                DatoTipoDocumento="";

            if(Rut=="null")
                DatoRut="";

            QString cabezal = "{\""+ClienteOProveedor+"\":\""+Cliente+"\",\"Comprobante\":\""+Comprobante+"\",\"Direccion\":\""+Direccion+"\",\"Emitir\":true "+DatoFecha+" ";

            QString pie = ""+DatoDocumento+",\"Moneda\":\""+Moneda+"\",\"Observaciones\":\""+Observacion+"\""+DatoPaisDocumento+",\"PorcentajeDescuento\":"+PorcentajeDescuento+",\"RazonSocial\":\""+RazonSocial+"\""+DatoRut+DatoTipoDocumento+",\"Total\":"+Total+"}";

            //,\"Serie\":\""+Serie+"\"

            //\"Numero\":\""+Numero+"\"

            QString Lineas= " ,\"Lineas\":[ ";

            query.previous();
            int i=0;
            QString codigoArticuloCargado="";

            while (query.next()){

                if(i!=0){
                    Lineas += " , ";
                }
                codigoArticuloCargado=query.value(15).toString();

                if(func_CFE_ParametrosGenerales.retornaValor("envioArticuloClienteGenerico")=="1"){
                    codigoArticuloCargado="1";
                }

                Lineas += "{\"Articulo\":\""+codigoArticuloCargado+"\",\"Cantidad\":"+query.value(17).toString()+",\"Descripcion\":\""+query.value(16).toString().toUpper().replace("Á","A").replace("É","E").replace("Í","I").replace("Ó","O").replace("Ú","U").replace("\"","\\\"").replace("\n","")+"\",\"Precio\":"+query.value(18).toString()+"}";
                i=1;

            }
            Lineas += "] ";

            QString ok=cabezal + Lineas + pie;

            return ok;//QString::fromUtf8(ok.toUtf8());


        }else{
            return "-1"; //No existe documento para enviar como CFE
        }
    }else{
        return "-2";//No se puede ejecutar la consulta a la base de datos
    }

}

