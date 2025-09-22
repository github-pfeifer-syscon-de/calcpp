
TESTS = calc_test

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS) \
	-I ../srcLib

AM_CFLAGS= \
	-Wall -Wpedantic

AM_CXXFLAGS = \
	-Wall -Wpedantic

AM_LDFLAGS =

calc_test_LDADD =  \
 	../srcLib/libexpression.a \
	$(GENERICIMG_LIBS)

check_PROGRAMS = calc_test

calc_test_SOURCES = \
    calc_test.cpp


