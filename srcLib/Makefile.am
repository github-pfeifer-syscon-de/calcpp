## Process this file with automake to produce Makefile.in
## for now a static lib is sufficent
## a shared lib woud reqire a seprate build and install

noinst_LIBRARIES = libexpression.a

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir). "\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS)

libexpression_a_CFLAGS = $(WARN_CFLAGS)
libexpression_a_CXXFLAGS = $(WARN_CXXFLAGS)

if RELEASE
libexpression_a_CFLAGS += $(OPTIM_CFLAGS)
libexpression_a_CXXFLAGS += $(OPTIM_CXXFLAGS)
else
libexpression_a_CXXFLAGS += $(INSTUMENT_CXXFLAGS)
endif

libexpression_a_SOURCES = \
	NumberFormat.cpp \
	NumberFormat.hpp \
	Token.cpp \
	Token.hpp \
	Function.cpp \
	Function.hpp \
	BaseEval.cpp \
	BaseEval.hpp \
	Syntax.cpp \
	Syntax.hpp

## include_HEADERS =

