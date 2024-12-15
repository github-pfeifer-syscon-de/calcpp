## Process this file with automake to produce Makefile.in
## for now a static lib is sufficent
## a shared lib woud reqire a seprate build and install

noinst_LIBRARIES = libexpression.a

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir). "\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS)

libexpression_a_CFLAGS = -Wall
libexpression_a_CXXFLAGS = -Wall

libexpression_a_SOURCES = \
	NumberFormat.cpp \
	NumberFormat.hpp \
	Token.cpp \
	Token.hpp \
	ConversionContext.cpp \
	ConversionContext.hpp \
	Syntax.cpp \
	Syntax.hpp

## include_HEADERS =

