dnl $Id$

dnl Report all available bsps for a target,
dnl check if a bsp-subdirectory is present for all bsps found
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN(RTEMS_CHECK_BSPS,
[
AC_REQUIRE([RTEMS_CHECK_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
AC_MSG_CHECKING([for bsps])
case "${target}" in
changequote(,)dnl
  i[3456]86-go32-rtems*)
changequote([,])dnl
    $1="go32 go32_p5"
    ;;
  *)
    files=`ls $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU`
    for file in $files; do
      case $file in
        shared*);;
        Makefile*);;
        READ*);;
        CVS*);;
        pxfl*);;
        go32*);;       # so the i386 port can pick up the other Makefiles
        # Now account for BSPs with build variants
        gen68360)      rtems_bsp="$rtems_bsp gen68360 gen68360_040";;
        p4000)         rtems_bsp="$rtems_bsp p4600 p4650";;
        mvme162)       rtems_bsp="$rtems_bsp mvme162 mvme162lx";;
        *) $1="[$]$1 $file";;
      esac;
    done
    ;;
esac
AC_MSG_RESULT([[$]$1 .. done])
])dnl

AC_DEFUN(RTEMS_CHECK_CUSTOM_BSP,
[dnl
AC_REQUIRE([RTEMS_TOP])

AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
if test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_ERROR([no])
fi
])dnl
