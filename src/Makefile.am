
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
	-Wno-psabi ## -Wno-psabi disable abi warn, as we use a value list..., may need changing if different gcc versions will be used

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
	QuadDialog.cpp \
	QuadDialog.hpp \
	GaussDialog.cpp \
	GaussDialog.hpp \
	DateDialog.cpp \
	DateDialog.hpp \
	Function.cpp \
	Function.hpp \
	Matrix.cpp \
	Matrix.hpp \
	PrefDialog.cpp \
	PrefDialog.hpp
