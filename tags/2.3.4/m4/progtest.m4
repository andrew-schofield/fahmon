# progtest.m4 serial 4 (gettext-0.14.2)
dnl Copyright (C) 1996-2003, 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl
dnl This file can can be used in projects which are not available under
dnl the GNU General Public License or the GNU Library General Public
dnl License but which still want to provide support for the GNU gettext
dnl functionality.
dnl Please note that the actual code of the GNU gettext library is covered
dnl by the GNU Library General Public License, and the rest of the GNU
dnl gettext package package is covered by the GNU General Public License.
dnl They are *not* in the public domain.

dnl Authors:
dnl   Ulrich Drepper <drepper@cygnus.com>, 1996.

AC_PREREQ(2.50)

AC_DEFUN([AM_PATH_PROG_WITH_TEST],
[
if test "${PATH_SEPARATOR+set}" != set; then
  echo "#! /bin/sh" >conf$$.sh
  echo  "exit 0"   >>conf$$.sh
  chmod +x conf$$.sh
  if (PATH="/nonexistent;."; conf$$.sh) >/dev/null 2>&1; then
    PATH_SEPARATOR=';'
  else
    PATH_SEPARATOR=:
  fi
  rm -f conf$$.sh
fi

cat >conf$$.file <<_ASEOF
#! /bin/sh
exit 0
_ASEOF
chmod +x conf$$.file
if test -x conf$$.file >/dev/null 2>&1; then
  ac_executable_p="test -x"
else
  ac_executable_p="test -f"
fi
rm -f conf$$.file

set dummy $2; ac_word=[$]2
AC_MSG_CHECKING([for $ac_word])
AC_CACHE_VAL(ac_cv_path_$1,
[case "[$]$1" in
  [[\\/]]* | ?:[[\\/]]*)
    ac_cv_path_$1="[$]$1" # Let the user override the test with a path.
    ;;
  *)
    ac_save_IFS="$IFS"; IFS=$PATH_SEPARATOR
    for ac_dir in ifelse([$5], , $PATH, [$5]); do
      IFS="$ac_save_IFS"
      test -z "$ac_dir" && ac_dir=.
      for ac_exec_ext in '' $ac_executable_extensions; do
        if $ac_executable_p "$ac_dir/$ac_word$ac_exec_ext"; then
          echo "$as_me: trying $ac_dir/$ac_word..." >&AS_MESSAGE_LOG_FD
          if [$3]; then
            ac_cv_path_$1="$ac_dir/$ac_word$ac_exec_ext"
            break 2
          fi
        fi
      done
    done
    IFS="$ac_save_IFS"
ifelse([$4], , , [  test -z "[$]ac_cv_path_$1" && ac_cv_path_$1="$4"
])dnl
    ;;
esac])dnl
$1="$ac_cv_path_$1"
if test ifelse([$4], , [-n "[$]$1"], ["[$]$1" != "$4"]); then
  AC_MSG_RESULT([$]$1)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST($1)dnl
])
