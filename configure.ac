AC_INIT([glthing], [0.0.0], [colin@kinlo.ch], [glthing])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability subdir-objects])

PKG_CHECK_MODULES([GTK], [gtkmm-3.0])
PKG_CHECK_MODULES([GL], [epoxy])

AC_CONFIG_FILES([
  Makefile
  src/res/glthing.gresource.xml
])

AC_OUTPUT
