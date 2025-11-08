
bin_PROGRAMS = calcpp

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
        -DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
        -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I"$(srcdir)"/../srcLib \
	$(GENERICIMG_CFLAGS) \
	$(GLIBMM_CFLAGS) \
	$(GTKMM_CFLAGS)

calcpp_CFLAGS = -Wall
calcpp_CXXFLAGS = -Wall \
	-Wpedantic \
	-Wconversion \
	-Wextra \
	-Wno-unused-parameter

calcpp_LDFLAGS = @EXTRA_LDFLAGS@

calcpp_LDADD =  \
	-lunistring \
 	../srcLib/libexpression.a \
	$(GENERICIMG_LIBS) \
	$(GLIBMM_LIBS) \
	$(GTKMM_LIBS)

calcpp_SOURCES = resources.c \
	CalcppApp.cpp \
	CalcppApp.hpp \
	CalcppWin.cpp \
	CalcppWin.hpp \
	EvalContext.cpp \
	EvalContext.hpp \
	OutputForm.cpp \
	OutputForm.hpp \
	AngleUnit.cpp \
	AngleUnit.hpp \
	CalcTextView.cpp \
	CalcTextView.hpp \
	CalcTreeView.cpp \
	CalcTreeView.hpp \
	CharDialog.cpp \
	CharDialog.hpp \
	NumDialog.cpp \
	NumDialog.hpp \
	NumBaseDialog.cpp \
	NumBaseDialog.hpp \
	QuadDialog.cpp \
	QuadDialog.hpp \
	GaussDialog.cpp \
	GaussDialog.hpp \
	DateDialog.cpp \
	DateDialog.hpp \
	Matrix.cpp \
	Matrix.hpp \
	Unit.cpp \
	Unit.hpp \
	UnitDialog.cpp \
	UnitDialog.hpp \
	PlotDialog.cpp \
	PlotDialog.hpp \
	QuadraticEquation.cpp \
	QuadraticEquation.hpp \
	PrefDialog.cpp \
	PrefDialog.hpp

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(pkgdatadir)

EXTRA_DIST = m4/ChangeLog