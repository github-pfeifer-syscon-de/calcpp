
TESTS = calc_test lin_test

check_PROGRAMS = calc_test lin_test

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS) \
	-I ../srcLib \
	-I ../src

AM_CFLAGS= \
	-Wall -Wpedantic

AM_CXXFLAGS = \
	-Wall \
	-Wconversion \
	-Wpedantic

AM_LDFLAGS =

calc_test_LDADD =  \
 	../srcLib/libexpression.a \
	../src/calcpp-Unit.o \
	../src/calcpp-NumDialog.o \
	$(GENERICIMG_LIBS)

calc_test_SOURCES = \
    calc_test.cpp

lin_test_LDADD =  \
	../src/calcpp-Matrix.o \
	../src/calcpp-Quad.o \
	$(GENERICIMG_LIBS)

lin_test_SOURCES = \
    lin_test.cpp