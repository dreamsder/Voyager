GRANT ALL ON almacen.* TO almacen@'%' IDENTIFIED BY 'almacen' ;
SET PASSWORD FOR almacen@'%'=OLD_PASSWORD('almacen');

GRANT ALL ON almacen.* TO almacen@'localhost' IDENTIFIED BY 'almacen' ;
SET PASSWORD FOR almacen@'localhost'=OLD_PASSWORD('almacen');

