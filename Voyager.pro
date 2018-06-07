# -------------------------------------------------
# Project created by QtCreator 2012-12-26T12:07:58
# -------------------------------------------------


QT       += core gui
QT       += network
QT       += sql



#greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
TARGET = Voyager
TEMPLATE = app
SOURCES += main.cpp \
    Utilidades/database.cpp \
    Utilidades/configuracionxml.cpp \
    Utilidades/logs.cpp \
    procesarArticulos.cpp \
    procesarlistadeprecios.cpp \
    procesarrubros.cpp \
    procesarsubrubros.cpp \
    procesarclientes.cpp \
    procesarclientesdesdekhitomer.cpp \
    procesarstock.cpp \
    procesarfacturas.cpp \
    CFE/modulo_cfe_parametrosgenerales.cpp \
    json/json.cpp \
    json/base64.cpp
HEADERS += Utilidades/database.h \
    Utilidades/configuracionxml.h \
    Utilidades/logs.h \
    procesarArticulos.h \
    procesarlistadeprecios.h \
    procesarrubros.h \
    procesarsubrubros.h \
    procesarclientes.h \
    procesarclientesdesdekhitomer.h \
    procesarstock.h \
    procesarfacturas.h \
    CFE/modulo_cfe_parametrosgenerales.h \
    json/json.hpp \
    json/base64.h \
    json/json.h \
    json/base64_nibbleandahalf.h
FORMS += 

win32:DEFINES  += WIN32
unix:DEFINES     += UNIX


unix:INCLUDEPATH += "/opt/Khitomer/lib"
unix:INCLUDEPATH += "/opt/Khitomer/lib/lib32"
unix:INCLUDEPATH += "/opt/Khitomer/lib/lib64"


unix:LIBS += -lBasicExcel -L"/opt/Khitomer/lib"  -lboost_system -lresolv -lpthread -lcurl  -lcrypt -lm -lcrypto
unix32:LIBS += -lchilkat-9.5.0 -L"/opt/Khitomer/lib/lib32"  -lboost_system -lresolv -lpthread -lcurl   -lcrypt -lm -lcrypto
unix64:LIBS += -lchilkat-9.5.0 -L"/opt/Khitomer/lib/lib64"  -lboost_system -lresolv -lpthread -lcurl   -lcrypt -lm -lcrypto

