AM_CFLAGS = 
bin_PROGRAMS = bbootstrap-server
#cxxmodulesdir=../cxxmodules/
#bacteriamodulesource=$(cxxmodulesdir)/bacteria/encdec.cpp $(cxxmodulesdir)/bacteria/cryptocurrency.cpp\
#	$(cxxmodulesdir)/bacteria/images.cpp $(cxxmodulesdir)/bacteria/bnet.cpp \
#	$(cxxmodulesdir)/bacteria/ed25519rsa.cpp 

bbootstrap_server_SOURCES = main.cpp ../net/utils.cpp ../net/opcodes.cpp serv.cpp 

#	$(bacteriamodulesource)
#$(top_builddir)/libbacteria/lua/luaserv.c
bbootstrap_server_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a $(top_builddir)/src/protocolcxx/libbprotocolcxx.a
bbootstrap_server_LDADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1 $(top_builddir)/src/protocolcxx/libbprotocolcxx.a
bbootstrap_server_CFLAGS= -g -Wno-pointer-sign

AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ -Wno-pointer-sign -std=c++2a -I$(top_srcdir)/src/protocolcxx/


target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




