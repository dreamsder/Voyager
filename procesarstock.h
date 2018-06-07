#ifndef PROCESARSTOCK_H
#define PROCESARSTOCK_H


#include <QString>


class ProcesarStock
{
public:
    static bool cargarStock();
    static bool actualizarStock(QString);
    static qlonglong retornaStock(QString);

private:
    ProcesarStock();
};

#endif // PROCESARSTOCK_H
