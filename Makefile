#############################################################################
# Makefile for building: Voyager
# Generated by qmake (2.01a) (Qt 4.8.7) on: jue jun 25 22:00:31 2020
# Project:  Voyager.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile Voyager.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DUNIX -DQT_NO_DEBUG -DQT_SQL_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtSql -I/usr/include/qt4 -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -lboost_system -lresolv -lcurl -lcrypt -lm -lcrypto -lQtSql -lQtNetwork -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
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
		json/base64.cpp moc_modulo_cfe_parametrosgenerales.cpp
OBJECTS       = main.o \
		database.o \
		configuracionxml.o \
		logs.o \
		procesarArticulos.o \
		procesarlistadeprecios.o \
		procesarrubros.o \
		procesarsubrubros.o \
		procesarclientes.o \
		procesarclientesdesdekhitomer.o \
		procesarstock.o \
		procesarfacturas.o \
		modulo_cfe_parametrosgenerales.o \
		json.o \
		base64.o \
		moc_modulo_cfe_parametrosgenerales.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		Voyager.pro
QMAKE_TARGET  = Voyager
DESTDIR       = 
TARGET        = Voyager

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: Voyager.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtSql.prl \
		/usr/lib/x86_64-linux-gnu/libQtNetwork.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile Voyager.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtSql.prl:
/usr/lib/x86_64-linux-gnu/libQtNetwork.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile Voyager.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/Voyager1.0.0 || $(MKDIR) .tmp/Voyager1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/Voyager1.0.0/ && $(COPY_FILE) --parents Utilidades/database.h Utilidades/configuracionxml.h Utilidades/logs.h procesarArticulos.h procesarlistadeprecios.h procesarrubros.h procesarsubrubros.h procesarclientes.h procesarclientesdesdekhitomer.h procesarstock.h procesarfacturas.h CFE/modulo_cfe_parametrosgenerales.h json/json.hpp json/base64.h json/json.h json/base64_nibbleandahalf.h .tmp/Voyager1.0.0/ && $(COPY_FILE) --parents main.cpp Utilidades/database.cpp Utilidades/configuracionxml.cpp Utilidades/logs.cpp procesarArticulos.cpp procesarlistadeprecios.cpp procesarrubros.cpp procesarsubrubros.cpp procesarclientes.cpp procesarclientesdesdekhitomer.cpp procesarstock.cpp procesarfacturas.cpp CFE/modulo_cfe_parametrosgenerales.cpp json/json.cpp json/base64.cpp .tmp/Voyager1.0.0/ && (cd `dirname .tmp/Voyager1.0.0` && $(TAR) Voyager1.0.0.tar Voyager1.0.0 && $(COMPRESS) Voyager1.0.0.tar) && $(MOVE) `dirname .tmp/Voyager1.0.0`/Voyager1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/Voyager1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_modulo_cfe_parametrosgenerales.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_modulo_cfe_parametrosgenerales.cpp
moc_modulo_cfe_parametrosgenerales.cpp: CFE/modulo_cfe_parametrosgenerales.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) CFE/modulo_cfe_parametrosgenerales.h -o moc_modulo_cfe_parametrosgenerales.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

main.o: main.cpp Utilidades/database.h \
		Utilidades/configuracionxml.h \
		procesarArticulos.h \
		Utilidades/logs.h \
		procesarlistadeprecios.h \
		procesarrubros.h \
		procesarsubrubros.h \
		procesarclientes.h \
		procesarclientesdesdekhitomer.h \
		procesarstock.h \
		procesarfacturas.h \
		CFE/modulo_cfe_parametrosgenerales.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

database.o: Utilidades/database.cpp Utilidades/database.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o database.o Utilidades/database.cpp

configuracionxml.o: Utilidades/configuracionxml.cpp Utilidades/configuracionxml.h \
		Utilidades/logs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o configuracionxml.o Utilidades/configuracionxml.cpp

logs.o: Utilidades/logs.cpp Utilidades/logs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o logs.o Utilidades/logs.cpp

procesarArticulos.o: procesarArticulos.cpp procesarArticulos.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarArticulos.o procesarArticulos.cpp

procesarlistadeprecios.o: procesarlistadeprecios.cpp procesarlistadeprecios.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarlistadeprecios.o procesarlistadeprecios.cpp

procesarrubros.o: procesarrubros.cpp procesarrubros.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarrubros.o procesarrubros.cpp

procesarsubrubros.o: procesarsubrubros.cpp procesarsubrubros.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarsubrubros.o procesarsubrubros.cpp

procesarclientes.o: procesarclientes.cpp procesarclientes.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarclientes.o procesarclientes.cpp

procesarclientesdesdekhitomer.o: procesarclientesdesdekhitomer.cpp procesarclientesdesdekhitomer.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarclientesdesdekhitomer.o procesarclientesdesdekhitomer.cpp

procesarstock.o: procesarstock.cpp procesarstock.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarstock.o procesarstock.cpp

procesarfacturas.o: procesarfacturas.cpp procesarfacturas.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h \
		curl/curl.h \
		curl/curlver.h \
		curl/system.h \
		curl/curlbuild.h \
		curl/curlrules.h \
		curl/easy.h \
		curl/multi.h \
		curl/typecheck-gcc.h \
		json/json.h \
		CFE/modulo_cfe_parametrosgenerales.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o procesarfacturas.o procesarfacturas.cpp

modulo_cfe_parametrosgenerales.o: CFE/modulo_cfe_parametrosgenerales.cpp CFE/modulo_cfe_parametrosgenerales.h \
		Utilidades/database.h \
		Utilidades/logs.h \
		Utilidades/configuracionxml.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o modulo_cfe_parametrosgenerales.o CFE/modulo_cfe_parametrosgenerales.cpp

json.o: json/json.cpp json/json.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o json.o json/json.cpp

base64.o: json/base64.cpp json/base64.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o base64.o json/base64.cpp

moc_modulo_cfe_parametrosgenerales.o: moc_modulo_cfe_parametrosgenerales.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_modulo_cfe_parametrosgenerales.o moc_modulo_cfe_parametrosgenerales.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:
