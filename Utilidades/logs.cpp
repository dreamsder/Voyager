#include "logs.h"
#include <QDir>
#include <QDate>
#include <QTime>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDebug>


Logs::Logs()
{
}

void Logs::loguear(QString _infoLog){

    QFile logDeInformacion(retornaDirectorioLog()+retornaNombreLog());
    QTextStream out(&logDeInformacion);

    if(!QDir(retornaDirectorioLog()).exists()){
        QDir directorio;
        if(!directorio.mkdir(retornaDirectorioLog())){

        }
    }

    if(QFile(retornaDirectorioLog()+retornaNombreLog()).exists()){
        logDeInformacion.open(QIODevice::Append);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+" ||>> "+_infoLog+"\n";
    }else{
        logDeInformacion.open(QIODevice::WriteOnly);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+" ||>> "+_infoLog+"\n";
    }
    logDeInformacion.close();

}

QString Logs::retornaDirectorioLog(){    
    if(QDir::rootPath()=="/"){
        return QDir::homePath()+"/logsVoyager/";
    }else{
        return QDir::rootPath()+"/Voyager/logs/";
    }
}
QString Logs::retornaNombreLog(){
    return "LogVoyager-"+QString::number(QDate::currentDate().day())+".log";
}

void Logs::depurarLog(){

    if(QFileInfo(retornaDirectorioLog()+retornaNombreLog()).lastModified().toString("yyyy-MM-dd")!=QDateTime::currentDateTime().toString("yyyy-MM-dd")){
        QFile(retornaDirectorioLog()+retornaNombreLog()).remove();
    }
}
