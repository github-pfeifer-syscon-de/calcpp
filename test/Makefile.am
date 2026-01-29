
TESTS = calc_test lin_test

check_PROGRAMS = calc_test lin_test

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICIMG_CFLAGS) \
	-I $(top_srcdir)/srcLib \
	-I $(top_srcdir)/src

calc_test_LDFLAGS = $(INSTUMENT_LINKFLAGS)

calc_test_LDADD = \
 	../srcLib/libexpression.a \
	../src/calcpp-Unit.o \
	../src/calcpp-NumDialog.o \
	$(GENERICIMG_LIBS)

calc_test_CXXFLAGS = $(WARN_CXXFLAGS) $(INSTUMENT_CXXFLAGS)

calc_test_SOURCES = \
    calc_test.cpp

lin_test_LDFLAGS = $(INSTUMENT_LINKFLAGS)

lin_test_LDADD = \
	../src/calcpp-Matrix.o \
	../src/calcpp-QuadraticEquation.o \
	$(GENERICIMG_LIBS)

lin_test_CXXFLAGS = $(WARN_CXXFLAGS) $(INSTUMENT_CXXFLAGS)

lin_test_SOURCES = \
    lin_test.cpp
