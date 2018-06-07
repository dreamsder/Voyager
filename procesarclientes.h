#ifndef PROCESARCLIENTES_H
#define PROCESARCLIENTES_H

#include <QString>

class ProcesarClientes
{
public:

    static bool cargaDeClientes();
    static bool insertarClientes(QString,QString,QString,QString,QString,QString , QString);
    static bool existeClienteEnBaseKhitomer(QString,QString);
    static bool actulizoClientesWeb(QString , QString , QString);
    static QString retornaUltimoCodigoClienteDisponible();
    static QString retornaListaPrecioCliente(QString);






private:
    ProcesarClientes();
};

#endif // PROCESARCLIENTES_H
