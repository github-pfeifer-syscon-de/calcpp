AC_INIT([calcpp program],[0.1],[calcpp@pfeifer-syscon.de])

AC_CONFIG_MACRO_DIR([m4])

dnl use -no-portablitity to stop complaining about $(shell... in res Makefile.am
dnl i dont get it, as the shell is everywhere, turtle power!

AM_INIT_AUTOMAKE([1.11 -Wno-portability])

AM_SILENT_RULES([yes])

AC_CONFIG_HEADERS([config.h])

AX_CHECK_ENABLE_DEBUG([info], [DEBUG], [RELEASE], [IS-RELEASE])
AC_PROG_CC
AC_PROG_CXX
dnl we do not depend on c++20 in just one place (glglobe),
dnl (did choose it for consistency across libs and programs)
dnl so if you happen to find yourself in the need to use
dnl a older compiler, c++17, should work as well.
AX_CXX_COMPILE_STDCXX([20], [noext], [optional])

GLIB_GSETTINGS

LT_INIT

AC_CANONICAL_HOST
AC_MSG_NOTICE([host_os $host_os])

PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])

PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 giomm-2.4])

AC_SEARCH_LIBS([uc_all_blocks], [unistring])

AX_CHECK_SED()

if test "$host_os" = "mingw32" ; then
    AC_SUBST(EXTRA_LDFLAGS, "-mwindows")
fi

AC_CONFIG_FILES([
Makefile
srcLib/Makefile
res/Makefile
src/Makefile])
AC_OUTPUT
