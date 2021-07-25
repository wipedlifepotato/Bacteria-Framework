#SUBDIRS=lua
AUTOMAKE_OPTIONS = subdir-objects
lib_LIBRARIES = libbacteria.a
libbacteria_a_SOURCES = cryptocoins.c  cryptocoins.h  json_rpc.c  lua/utils.c \
		       	lua/afirst.c  lua/libcrypto.c  lua/libjson.c lua/libencdec.c lua/libimages.c lua/libgmp.c lua/libed25519rsa.c lua/libnet.c \
			encdec/async/x25519.c encdec/sync/AES.c encdec/verify/rsa_ed25519.c encdec/verify/base64.c encdec/hashes.c \
			images/images.c net.c \
			lua/luaserv.c
libbacteria_a_CFLAGS = -g -I /usr/include/luajit-2.1/ -I$(topdir)/libbacteria/ -I$(topdir)/luasql/src -I/usr/include/lua5.1/ -Wno-pointer-sign

