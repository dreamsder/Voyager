#ifndef CONFIGURACIONXML_H
#define CONFIGURACIONXML_H

#include <QString>

class ConfiguracionXml
{
public:

    static bool leerConfiguracionXml();

    static QString getUsuarioRemoto(){
        return m_usuarioRemoto;
    }
    static QString getClaveRemoto(){
        return m_claveRemoto;
    }
    static QString getBaseRemoto(){
        return m_baseRemoto;
    }
    static int getPuertoRemoto(){
        return m_puertoRemoto;
    }
    static QString getHostRemoto(){
        return m_hostRemoto;
    }
    static QString getUsuarioLocal(){
        return m_usuarioLocal;
    }
    static QString getClaveLocal(){
        return m_claveLocal;
    }
    static QString getBaseLocal(){
        return m_baseLocal;
    }
    static int getPuertoLocal(){
        return m_puertoLocal;
    }
    static QString getHostLocal(){
        return m_hostLocal;
    }


private:
    ConfiguracionXml();

    static QString m_usuarioRemoto;
    static QString m_claveRemoto;
    static QString m_baseRemoto;
    static int m_puertoRemoto;
    static QString m_hostRemoto;

    static QString m_usuarioLocal;
    static QString m_claveLocal;
    static QString m_baseLocal;
    static int m_puertoLocal;
    static QString m_hostLocal;


    static void setUsuarioRemoto(QString value){
        m_usuarioRemoto=value;
    }
    static void setClaveRemoto(QString value){
        m_claveRemoto=value;
    }
    static void setBaseRemoto(QString value){
        m_baseRemoto=value;
    }
    static void setPuertoRemoto(int value){
        m_puertoRemoto=value;
    }
    static void setHostRemoto(QString value){
        m_hostRemoto=value;
    }
    static void setUsuarioLocal(QString value){
        m_usuarioLocal=value;
    }
    static void setClaveLocal(QString value){
        m_claveLocal=value;
    }
    static void setBaseLocal(QString value){
        m_baseLocal=value;
    }
    static void setPuertoLocal(int value){
        m_puertoLocal=value;
    }
    static void setHostLocal(QString value){
        m_hostLocal=value;
    }


};

#endif // CONFIGURACIONXML_H
