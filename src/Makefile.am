SUBDIRS= protocolcxx client
AM_CFLAGS = 
bin_PROGRAMS = bacteria
#bacteriamodulesource=cxxmodules/bacteria/encdec.cpp cxxmodules/bacteria/cryptocurrency.cpp\
#	cxxmodules/bacteria/images.cpp cxxmodules/bacteria/bnet.cpp \
#	cxxmodules/bacteria/ed25519rsa.cpp 

bacteria_SOURCES = async_serv.cpp main.cpp signal_handler.cpp net/opcodes.cpp
bacteria_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a
bacteria_LDADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1 
bacteria_CFLAGS= -g -Wno-pointer-sign

AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ -Wno-pointer-sign -std=c++2a


target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




