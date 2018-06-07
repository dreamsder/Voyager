#ifndef PROCESARSUBRUBROS_H
#define PROCESARSUBRUBROS_H

#include <QString>


class ProcesarSubRubros
{
public:

    static bool cargaDeListaDeSubRubros();
    static bool insertoListaSubRubros(QString , QString, QString);
    static bool existeSubRubroEnWeb(QString ,QString );
    static void marcarSubRubrosComoSincronizado(QString ,QString );



private:
    ProcesarSubRubros();


};

#endif // PROCESARSUBRUBROS_H
