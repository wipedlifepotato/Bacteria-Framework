AM_CFLAGS = 
bin_PROGRAMS = bbootstrap-server
cxxmodulesdir=../cxxmodules/
bacteriamodulesource=$(cxxmodulesdir)/bacteria/encdec.cpp $(cxxmodulesdir)/bacteria/cryptocurrency.cpp\
	$(cxxmodulesdir)/bacteria/images.cpp $(cxxmodulesdir)/bacteria/net.cpp \
	$(cxxmodulesdir)/bacteria/ed25519rsa.cpp \
	$(cxxmodulesdir)/bacteria/json.cpp

bbootstrap_server_SOURCES = main.cpp ../net/utils.cpp ../net/opcodes.cpp serv.cpp \
	$(bacteriamodulesource)
#$(top_builddir)/libbacteria/lua/luaserv.c
ibootstrap_server_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a
bbootstrap_server_LDADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1
ibootstrap_server_CFLAGS= -g -Wno-pointer-sign

AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ -fmodules-ts -Wno-pointer-sign


target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




