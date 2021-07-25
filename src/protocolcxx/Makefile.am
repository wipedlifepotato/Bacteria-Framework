noinst_LIBRARIES = libbprotocolcxx.a
libbprotocolcxx_a_SOURCES = bnet.cpp ed25519rsa.cpp images.cpp cryptocurrency.cpp encdec.cpp
libbprotocolcxx_a_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a
#libprotocolcxx_a_LIBADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1 
#libprotocolcxx_a_CFLAGS= -g -Wno-pointer-sign -lcrypto -lssl
AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ -std=c++2a 

target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




