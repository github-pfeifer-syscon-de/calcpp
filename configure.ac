dnl Process this file with autoconf to produce a configure script.

AC_CONFIG_MACRO_DIR([m4])

AC_INIT([calcpp],[0.1])

dnl point to unique-file-in-source-dir, for safety check
AC_CONFIG_SRCDIR([src/CalcppApp.cpp])

dnl foreign -> leaves out INSTALL,
dnl otherwise -> there is complaining about the shell, sorry for all non shell places :(
AM_INIT_AUTOMAKE([1.11])

AC_CONFIG_HEADERS([config.h])

AM_SILENT_RULES([yes])

AX_CHECK_ENABLE_DEBUG([info], [DEBUG], [RELEASE], [IS-RELEASE])
AC_PROG_CC
AC_PROG_CXX
dnl did choose c++20 for consistency across libs and programs
dnl so if you happen to find yourself in the need to use
dnl a older compiler, c++17 may need some adaptions.
AX_CXX_COMPILE_STDCXX([20], [noext], [optional])

GLIB_GSETTINGS

LT_INIT

AC_CANONICAL_HOST
AC_MSG_NOTICE([host_os $host_os])

dnl don't repeat dependencies
dnl PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])
dnl PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 giomm-2.4])

PKG_CHECK_MODULES([GENERICIMG],[genericimg >= 0.4.0])

AC_SEARCH_LIBS([uc_all_blocks], [unistring])

AM_GNU_GETTEXT([external])

if test "$host_os" = "mingw32" ; then
    AC_SUBST(EXTRA_LDFLAGS, "-mwindows")
fi

AC_CONFIG_FILES([
po/Makefile.in
Makefile
srcLib/Makefile
test/Makefile
res/Makefile
src/Makefile
])
AC_OUTPUT
