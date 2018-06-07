#ifndef PROCESARLISTADEPRECIOS_H
#define PROCESARLISTADEPRECIOS_H

#include <QString>

class ProcesarListaDePrecios
{
public:



    static bool cargaDeListaDePrecios();
    static bool insertoListaPrecio(QString , QString, double );
    static bool existeListaPrecioEnWeb(QString ,QString );
    static void marcarListaPreciosComoSincronizado(QString ,QString );



private:
    ProcesarListaDePrecios();


};

#endif // PROCESARLISTADEPRECIOS_H
