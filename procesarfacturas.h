#ifndef PROCESARFACTURAS_H
#define PROCESARFACTURAS_H

#include <QString>

class ProcesarFacturas
{
public:
    static bool cargaDeFacturas();

    static bool guardarFactura(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);

    static  qlonglong retornoCodigoUltimoDocumentoDisponible();
    static QString retornoSerieDelDocumento();
    static double retornoCotizacionMoneda();
    static QString retornoUltimaLiquidacionActiva();
    static QString retornoVendedorDeLiquidacion(QString);
    static  bool guardarLineaDocumento(QString );
    static double retornoFactorMultiplicador(QString );
    static int retornoCodigoIva(QString );
    static bool actualizarDatosDocumento(QString );
    static bool actualizarDatoFacturaEnLaWeb(QString ,QString );
    static QString retornaDireccionWebFactura(QString ,QString );

    static QString retornaValorConfiguracion(QString );
    static  bool retornaClienteTieneRUT(QString ,QString );
    static QString retornaDescripcionArticulo(QString );

    static  bool actualizarEstadoDocumentoAEmitido(QString);


    static bool emitirDocumentoCFEImix(QString _codigoDocumento,QString _codigoTipoDocumento, QString _descripcionEstadoActualDocumento);

    QString crearJsonIMIX(QString _codigoDocumento,QString _codigoTipoDocumento);





private:
    ProcesarFacturas();
};

#endif // PROCESARFACTURAS_H
