dnl Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
dnl rwlock code added by Mike Blumenkrantz <mike at zentific dot com>
dnl This code is public domain and can be freely used or copied.

dnl Macro that check if POSIX or Win32 threads library is available or not.

dnl Usage: EFL_CHECK_THREADS(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
dnl Call AC_SUBST(EFL_PTHREAD_CFLAGS)
dnl Call AC_SUBST(EFL_PTHREAD_LIBS)
dnl Defines EFL_HAVE_POSIX_THREADS or EFL_HAVE_WIN32_THREADS, and EFL_HAVE_THREADS

AC_DEFUN([EFL_CHECK_THREADS],
[

dnl Generic thread detection

dnl Use generic infrastructure for pthread detection (What a hell of a mess !)
_efl_have_ax_threads="no"
AX_PTHREAD([
  EFL_PTHREAD_CFLAGS="${PTHREAD_CFLAGS}"
  EFL_PTHREAD_LIBS="${PTHREAD_LIBS}"
  CC="${PTHREAD_CC}"
  _efl_have_ax_threads="yes"
  ])

_efl_have_posix_threads="no"
_efl_have_win32_threads="no"

AC_DEFINE([EFL_HAVE_THREADS], [1], [Define to mention that POSIX or Win32 threads are supported])

case "$host_os" in
   mingw*)
      _efl_have_win32_threads="yes"
      efl_have_setaffinity="yes"
      AC_DEFINE([EFL_HAVE_WIN32_THREADS], [1], [Define to mention that Win32 threads are supported])
      ;;
   *)
      _efl_have_posix_threads="${_efl_have_ax_threads}"
      AC_DEFINE([EFL_HAVE_POSIX_THREADS], [1], [Define to mention that POSIX threads are supported])
      ;;
esac

dnl check if the compiler supports POSIX thread barrier and setaffinity
if test "x${_efl_have_posix_threads}" = "xyes" ; then

   SAVE_LIBS=${LIBS}
   SAVE_CFLAGS=${CFLAGS}
   LIBS="${LIBS} ${PTHREAD_LIBS}"
   CFLAGS="${CFLAGS} ${PTHREAD_CFLAGS}"
   AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[
#include <pthread.h>
                       ]],
                       [[
pthread_barrier_t barrier;
pthread_barrier_init(&barrier, NULL, 1);
                       ]])],
      [efl_have_pthread_barrier="yes"],
      [efl_have_pthread_barrier="no"])
   AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
                       ]],
                       [[
pthread_attr_setaffinity_np(NULL, 0, NULL);
                       ]])],
      [efl_have_setaffinity="yes"],
      [efl_have_setaffinity="no"])
   LIBS=${SAVE_LIBS}
   CFLAGS=${SAVE_CFLAGS}
fi

AC_MSG_CHECKING([which threads API is used])
if test "x${_efl_have_posix_threads}" = "xyes" ; then
   efl_have_threads="POSIX"
else
   if test "x${_efl_have_win32_threads}" = "xyes" ; then
      efl_have_threads="Windows"
      efl_have_pthread_barrier="no"
   else
      efl_have_threads="no"
      efl_have_pthread_barrier="no"
   fi
fi
AC_MSG_RESULT([${efl_have_threads}])

AC_SUBST(EFL_PTHREAD_CFLAGS)
AC_SUBST(EFL_PTHREAD_LIBS)

dnl check if the compiler supports pthreads spinlock

efl_have_posix_threads_spinlock="no"

if test "x${_efl_have_posix_threads}" = "xyes" ; then

   SAVE_LIBS=${LIBS}
   SAVE_CFLAGS=${CFLAGS}
   LIBS="${LIBS} ${PTHREAD_LIBS}"
   CFLAGS="${CFLAGS} ${PTHREAD_CFLAGS}"
   AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[
#include <pthread.h>
#include <sched.h>
                       ]],
                       [[
pthread_spinlock_t lock;
int res;
res = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
sched_yield();
                       ]])],
      [efl_have_posix_threads_spinlock="yes"],
      [efl_have_posix_threads_spinlock="no"])
   LIBS=${SAVE_LIBS}
   CFLAGS=${SAVE_CFLAGS}

fi

AC_MSG_CHECKING([whether to build POSIX threads spinlock code])
AC_MSG_RESULT([${efl_have_posix_threads_spinlock}])

if test "x${efl_have_posix_threads_spinlock}" = "xyes" ; then
   AC_DEFINE([EFL_HAVE_POSIX_THREADS_SPINLOCK], [1], [Define to mention that POSIX threads spinlocks are supported])
fi

AS_IF([test "x$_efl_have_posix_threads" = "xyes" || test "x$_efl_have_win32_threads" = "xyes"],
   [$1],
   [m4_if([$2], [$2], [AC_MSG_ERROR([Threads are required.])])])

])
