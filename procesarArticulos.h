#ifndef PROCESARINFORMACION_H
#define PROCESARINFORMACION_H

#include <QString>

class ProcesarArticulos
{
public:



    static bool cargaDeArticulos();
    static bool existeArticuloEnBaseWeb(QString);
    static bool actualizoArticulo(QString, QString, QString );
    static  bool actualizoArticuloDescripcion(QString , QString );

    static bool insertoArticulo(QString, QString, QString );
    static bool insertoArticuloDescripcion(QString,QString);

    static void marcarArticuloComoSincronizado(QString);


  //  static int EjectuarComando(QString);
  //  static void informeDeCargaDeDatosVoyager(QString);



private:
    ProcesarArticulos();


};
#endif // PROCESARINFORMACION_H
