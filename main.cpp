#include <QApplication>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>
#include <Utilidades/database.h>
#include <Utilidades/configuracionxml.h>
#include <procesarArticulos.h>
#include <QDateTime>
#include <Utilidades/logs.h>
#include <procesarlistadeprecios.h>
#include <procesarrubros.h>
#include <procesarsubrubros.h>
#include <procesarclientes.h>
#include <procesarclientesdesdekhitomer.h>
#include <procesarstock.h>
#include <procesarfacturas.h>

#include <CFE/modulo_cfe_parametrosgenerales.h>

// Funcion para mostrar la ayuda
void muestroAyuda();

// Variables de entorno
QString _accion="";
QString _version="1.6.3";



int main(int argc, char *argv[])
{

    /// Borra los logs viejos del mismo dia
    Logs::depurarLog();

    /// Codificacion del sistema para aceptar tildes y ñ
    QTextCodec *linuxCodec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);

    Modulo_CFE_ParametrosGenerales confCFE;


    /// Si no hay argumentos arranco la aplicación en modo GUI
    if(argc==1){

        muestroAyuda();

    }else{

        /// Chequeo el dia de funcionamiento del cargador no sea sabado, ni domingo
        if(QDateTime::currentDateTime().date().dayOfWeek()!=6 &&  QDateTime::currentDateTime().date().dayOfWeek()!=7){

            /// Chequeo que el horario de funcionamiento del cargador sea mayor a las 8:00 y menor a las 19:00 hs.
            if(QDateTime::currentDateTime().toString("HH").toInt() > 7 && QDateTime::currentDateTime().toString("HH").toInt() < 19){

                QApplication app(argc, argv);
                //QCoreApplication app(argc, argv);
                Logs::loguear("");
                Logs::loguear("");
                Logs::loguear("___________________________________________________________________________");
                Logs::loguear("Inicio de aplicación de carga de Voyager [MODO CONSOLA], ver: "+_version);


                /// Hago la evaluacion de argumentos
                for (int i=0; i< argc; i++){
                    QString _argumentos(argv[i]);

                    if(_argumentos.compare("--help",Qt::CaseInsensitive)==0 || _argumentos.compare("--ayuda",Qt::CaseInsensitive)==0 || _argumentos.compare("-?",Qt::CaseInsensitive)==0 || _argumentos.compare("-version",Qt::CaseInsensitive)==0 || _argumentos.compare("-v",Qt::CaseInsensitive)==0 || _argumentos.compare("--version",Qt::CaseInsensitive)==0){

                        _accion="ayuda";
                        break;

                    }else if(_argumentos.compare("batch",Qt::CaseSensitive)==0){

                        _accion="batch";

                    }
                }

                if(_accion=="ayuda"){
                    Logs::loguear("Muestra la ayuda y se cierra la aplicación");
                    muestroAyuda();
                    exit(0);
                }else if(_accion=="batch"){
                    if(!ConfiguracionXml::leerConfiguracionXml()){
                        Logs::loguear("Muestra la ayuda y se cierra la aplicación");
                        muestroAyuda();
                        exit(0);
                    }

                    confCFE.cargar();

                    QString HoraComienzo =QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
                    Logs::loguear("\tFecha hora comienzo: "+HoraComienzo);

                    Logs::loguear("1 - Comienzo la carga de rubros: ");
                    if(ProcesarRubros::cargaDeListaDeRubros()){
                        Logs::loguear("\tRubros cargados OK.");

                        Logs::loguear("2 - Comienzo la carga de subrubros: ");
                        if(ProcesarSubRubros::cargaDeListaDeSubRubros()){
                            Logs::loguear("\tSubRubros cargados OK.");

                            Logs::loguear("3 - Comienzo la carga de artículos: ");
                            if(ProcesarArticulos::cargaDeArticulos()){
                                Logs::loguear("\tArtículos cargados OK.");

                                Logs::loguear("4 - Comienzo la carga de precios: ");
                                if(ProcesarListaDePrecios::cargaDeListaDePrecios()){
                                    Logs::loguear("\tPrecios cargados OK.");

                                    Logs::loguear("5 - Comienzo la carga de clientes desde la web: ");
                                    if(ProcesarClientes::cargaDeClientes()){
                                        Logs::loguear("\tClientes cargados OK.");

                                        Logs::loguear("6 - Comienzo la carga de actualizaciónes de clientes desde Khitomer: ");
                                        if(ProcesarClientesDesdeKhitomer::cargaDeClientes()){
                                            Logs::loguear("\tClientes desde Khitomer cargados OK.");

                                            Logs::loguear("7 - Comienzo la carga de stock desde Khitomer: ");
                                            if(ProcesarStock::cargarStock()){
                                                Logs::loguear("\tStock cargado OK.");


                                                Logs::loguear("8 - Carga de facturas desde la web: ");
                                                if(ProcesarFacturas::cargaDeFacturas()){
                                                    Logs::loguear("\tFacturas cargadas OK.");



                                                    Logs::loguear("\tFin del proceso de carga.");



                                                }else{
                                                    Logs::loguear("No se pudieron cargar facturas");
                                                }
                                            }else{
                                                Logs::loguear("No se pudo cargar el stock");
                                            }
                                        }else{
                                            Logs::loguear("No se pudieron cargaror los clientes desde Khitomer");
                                        }
                                    }else{
                                        Logs::loguear("No se pudieron cargaror los clientes");
                                    }
                                }else{
                                    Logs::loguear("No se pudieron cargaror los precios");
                                }
                            }else{
                                Logs::loguear("No se puedieron cargar los artículos");
                            }
                        }else{
                            Logs::loguear("No se puedieron cargar los subrubros");
                        }
                    }else{
                        Logs::loguear("No se puedieron cargar los rubros");
                    }
                }else if(_accion==""){
                    muestroAyuda();
                    Logs::loguear("Muestra la ayuda y se cierra la aplicación");
                    exit(0);
                }
            }}
    }
}

/// Funcion que muestra el ayuda de la aplicación
void muestroAyuda(){









    qDebug()<< "###################################################\n\tVoyager - Versión "+_version+"\n###################################################";
    qDebug()<< "\nFORMAS DE USO:";
    qDebug()<< "\n* Para ejecutar la aplicación, pase como parametro 'batch'.\n  Ejemplo: ./Voyager batch";
    qDebug()<< "\n* Para visualizar esta ayuda, pase como parametro [--ayuda, --help, -?, -version, --version, -v].\n  Ejemplo: ./Voyager --ayuda";
    qDebug()<< "\nHistorico de versiones:";
    qDebug()<< "\t1.0.0 - Versión inicial del sincronizador. Soporte de artículos, clientes, listas de precio, rubros, sub rubros y stock.";
    qDebug()<< "\t1.0.1 - Corrección en sincronización de clientes con rut, si ya existian en Khitomer y tenian listas de precios asignadas.";
    qDebug()<< "\t1.1.0 - Se agrega soporte para impresión de facturas.";
    qDebug()<< "\t1.2.0 - Se modifica las consultas, actualizaciones e inserciones de datos para optimizar el funcionamiento.";
    qDebug()<< "\t        Se modifica la consulta de carga de precios para pasar los artículos a dolares, si estan en pesos.";
    qDebug()<< "\t1.3.0 - Se modifica las consultas, para contemplar todos los datos de la tabla productos en la actualización del stock.";
    qDebug()<< "\t        Se modifica la consulta de carga de facturas para contemplar posibles registros en null, o vacio.";
    qDebug()<< "\t1.3.1 - Se corrige un error en la insercion del cliente.";
    qDebug()<< "\t1.3.2 - Se mejora la velocidad en la carga de stock.";
    qDebug()<< "\t1.3.3 - Se loguea mas información a la hora de imprimir un documento.";
    qDebug()<< "\t1.3.4 - Se modifica la dirección a grabar en la factura para quitar el telefono, y el departamento y ciudad si son de montevideo";
    qDebug()<< "\t1.4.0 - Se modifica la sincronización de artículos para que la inactivación se realice desde el programa khitomer.";
    qDebug()<< "\t1.4.1 - Se corrige la impresión de la fecha del documento para que salga en formato español y no en ingles.";
    qDebug()<< "\t1.5.0 - Se modifica la sincronización de clientes para contemplar la existencia de clientes por su email o rut.";
    qDebug()<< "\t1.6.0 - Actualización a CFE";
    qDebug()<< "\t1.6.1 - Se corrige en el envio de CFE, string que pudieran tener saltos de linea.";
    qDebug()<< "\t      - Se corrige el grabado de los creditos, ya que no aparecian con deuda en el sistema.";
    qDebug()<< "\t1.6.2 - 27/07/2018 - Se agrega como vendedor para comisionar por defecto a Giseel a pedido de Fernando Buero.";
    qDebug()<< "\t1.6.3 - 22/03/2019 - Se desagrega la cantidad de items en una factura, para que desde la web siempre vaya como una unidad en ligar de 2 o 3 o mas.";

    qDebug()<< "";










}
