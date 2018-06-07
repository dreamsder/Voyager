#ifndef PROCESARCLIENTESDESDEKHITOMER_H
#define PROCESARCLIENTESDESDEKHITOMER_H

#include <QString>

class ProcesarClientesDesdeKhitomer
{
public:
    static bool cargaDeClientes();    
    static bool actulizoClientesWeb(QString  , QString);
    static QString retornaListaPrecioCliente(QString);
    static void marcarClienteComoSincronizado(QString);


private:
    ProcesarClientesDesdeKhitomer();
};

#endif // PROCESARCLIENTESDESDEKHITOMER_H
