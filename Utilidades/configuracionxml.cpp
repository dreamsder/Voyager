#include "configuracionxml.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QString>
#include <QDir>
#include <Utilidades/logs.h>


QString ConfiguracionXml::m_usuarioRemoto;
QString ConfiguracionXml::m_baseRemoto;
QString ConfiguracionXml::m_claveRemoto;
QString ConfiguracionXml::m_hostRemoto;
int ConfiguracionXml::m_puertoRemoto;

QString ConfiguracionXml::m_usuarioLocal;
QString ConfiguracionXml::m_baseLocal;
QString ConfiguracionXml::m_claveLocal;
QString ConfiguracionXml::m_hostLocal;
int ConfiguracionXml::m_puertoLocal;

QString filename;

ConfiguracionXml::ConfiguracionXml()
{

}

bool ConfiguracionXml::leerConfiguracionXml(){

    if(QDir::rootPath()=="/"){
        filename = "/opt/Voyager/conf.xml";
    }else{
        filename = QDir::rootPath()+"/Voyager/conf.xml";
    }
    if(QFile::exists(filename)){

        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {            
            Logs::loguear(" ERROR   ->>  No se pudo leer el archivo de configuración: " +filename);
            return false;
        }

        QXmlStreamReader read;

        read.setDevice(&file);
        read.readNext();

        while(!read.atEnd()){

            if(read.isStartElement())
            {
                if(read.name() == "CONFIGURACION")
                {
                    read.readNext();

                }else if(read.name() == "ACCESO_REMOTO"){

                    read.readNext();

                }else if(read.name() == "usuario_R"){

                    ConfiguracionXml::setUsuarioRemoto(read.readElementText());

                }else if(read.name() == "clave_R"){

                    ConfiguracionXml::setClaveRemoto(read.readElementText());

                }else if(read.name() == "base_R"){

                    ConfiguracionXml::setBaseRemoto(read.readElementText());

                }else if(read.name() == "host_R"){

                    ConfiguracionXml::setHostRemoto(read.readElementText());

                }else if(read.name() == "puerto_R"){

                    ConfiguracionXml::setPuertoRemoto(read.readElementText().toInt());

                }else if(read.name() == "ACCESO_LOCAL"){

                    read.readNext();

                }else if(read.name() == "usuario_L"){

                    ConfiguracionXml::setUsuarioLocal(read.readElementText());

                }else if(read.name() == "clave_L"){

                    ConfiguracionXml::setClaveLocal(read.readElementText());

                }else if(read.name() == "base_L"){

                    ConfiguracionXml::setBaseLocal(read.readElementText());

                }else if(read.name() == "host_L"){

                    ConfiguracionXml::setHostLocal(read.readElementText());

                }else if(read.name() == "puerto_L"){

                    ConfiguracionXml::setPuertoLocal(read.readElementText().toInt());

                }else{
                    read.readNext();
                }
            }else{
                read.readNext();
            }
        }
        file.close();

        if(ConfiguracionXml::getBaseLocal()!="")
            return true;

    }else{        
        Logs::loguear(" ERROR   ->>  No existe el archivo de configuración: " +filename);
        return false;
    }
}
