SUBDIRS=bootstrap client
AM_CFLAGS = 
bin_PROGRAMS = bacteria
bacteriamodulesource=cxxmodules/bacteria/encdec.cpp cxxmodules/bacteria/cryptocurrency.cpp\
	cxxmodules/bacteria/images.cpp cxxmodules/bacteria/net.cpp \
	cxxmodules/bacteria/ed25519rsa.cpp cxxmodules/bacteria/gmp.cpp \
	cxxmodules/bacteria/json.cpp

bacteria_SOURCES = async_serv.cpp main.cpp signal_handler.cpp $(bacteriamodulesource)
bacteria_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a
bacteria_LDADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1 
bacteria_CFLAGS= -g -Wno-pointer-sign

AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ -fmodules-ts -Wno-pointer-sign


target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




