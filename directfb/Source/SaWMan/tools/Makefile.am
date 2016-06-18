## Makefile.am for SaWMan/tools

INCLUDES = \
	-I$(top_builddir)/include	\
	-I$(top_builddir)/src		\
	-I$(top_srcdir)/include		\
	-I$(top_srcdir)/src		\
	$(DFB_CFLAGS)


bin_PROGRAMS = swmdump


swmdump_SOURCES = swmdump.c
swmdump_LDADD   = $(DFB_LIBS) ../src/libsawman.la
