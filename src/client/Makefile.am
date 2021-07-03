AM_CFLAGS = 
bin_PROGRAMS = bacteria-client
bacteria_client_SOURCES = main.cpp ../bootstrap/utils.cpp
bacteria_client_DEPENDENCIES = $(top_builddir)/libbacteria/libbacteria.a
bacteria_client_LDADD = $(top_builddir)/libbacteria/libbacteria.a -lluajit-5.1 $(GTK_LIBS)
bacteria_client_CFLAGS= -g 
AM_CPPFLAGS = -I$(top_srcdir)/libbacteria/ -I/usr/include/lua5.1/ -I$(top_builddir)/libbacteria -I/usr/include/lua5.1/ -I /usr/include/luajit-2.1/ $(GTK_CFLAGS)



target_CPPFLAGS = -I$(top_srcdir)/libbacteria/




