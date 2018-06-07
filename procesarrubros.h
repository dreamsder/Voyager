#ifndef PROCESARRUBROS_H
#define PROCESARRUBROS_H

#include <QString>

class ProcesarRubros
{
public:

    static bool cargaDeListaDeRubros();
    static bool insertoListaRubros(QString , QString);
    static bool existeRubroEnWeb(QString );
    static void marcarRubrosComoSincronizado(QString );


private:
    ProcesarRubros();


};

#endif // PROCESARRUBROS_H
