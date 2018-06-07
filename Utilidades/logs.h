#ifndef LOGS_H
#define LOGS_H
#include <QString>

class Logs
{
public:
    static void loguear(QString);

    static QString retornaNombreLog();
    static QString retornaDirectorioLog();

    static void depurarLog();

private:
    Logs();
};

#endif // LOGS_H
