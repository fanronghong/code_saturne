dnl----------------------------------------------------------------------------
dnl   This file is part of the Code_Saturne Kernel, element of the
dnl   Code_Saturne CFD tool.
dnl
dnl   Copyright (C) 2009 EDF S.A., France
dnl
dnl   The Code_Saturne Kernel is free software; you can redistribute it
dnl   and/or modify it under the terms of the GNU General Public License
dnl   as published by the Free Software Foundation; either version 2 of
dnl   the License, or (at your option) any later version.
dnl
dnl   The Code_Saturne Kernel is distributed in the hope that it will be
dnl   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
dnl   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public Licence
dnl   along with the Code_Saturne Preprocessor; if not, write to the
dnl   Free Software Foundation, Inc.,
dnl   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
dnl-----------------------------------------------------------------------------

# CS_AC_TEST_FC_MOD([use_threads])
#------------------
# checks how the Fortran compiler handles modules

AC_DEFUN([CS_AC_TEST_FC_MOD], [

cs_fc_modext=""
cs_fc_modflag=""

AC_LANG_PUSH(Fortran)

# Create temporary directory

i=0
while test \( -f tmpdir_$i \) -o \( -d tmpdir_$i \) ;
do
  i=`expr $i + 1`
done
mkdir tmpdir_$i

# Compile module in temporary directory and check extension
# (some compilers put module filename in uppercase letters,
# so also check this)

cd tmpdir_$i
AC_COMPILE_IFELSE([
      module conftest_module
      contains
      subroutine conftest_routine
      write(*,'(a)') 'conftest'
      end subroutine conftest_routine
      end module conftest_module
  ],
                  [cs_fc_modext=`ls | sed -n 's,conftest_module\.,,p'`
                   if test x$cs_fc_modext = x ; then
                     cs_fc_modext=`ls | sed -n 's,CONFTEST_MODULE\.,,p'`
                     if test x$cs_fc_modext = x ; then
                       cs_fc_modext=""
                     fi
                   fi
                  ],
                  [cs_fc_modext=""])

# Go up one level and search for module using probable flags

cd ..

for cs_fc_flag in "-I " "-M" "-p"; do
  if test "x$cs_fc_modflag" = "x" ; then
    save_FCFLAGS="$FCFLAGS"
    FCFLAGS="$save_FCFLAGS ${cs_fc_flag}tmpdir_$i"
    AC_COMPILE_IFELSE([
      program conftest_program
      use conftest_module
      call conftest_routine
      end program conftest_program
      ],
                      [cs_fc_modflag="$cs_fc_flag"],
                      [])
    FCFLAGS="$save_FCFLAGS"
  fi
done

# Now remove temporary directory and finish

rm -fr tmpdir_$i
AC_LANG_POP(Fortran)

])dnl

# CS_AC_TEST_FC_FLUSH
#------------------
# checks if the Fortran compiler handles flush (Fortran 2003)

AC_DEFUN([CS_AC_TEST_FC_FLUSH], [

cs_fc_flush=no

AC_LANG_PUSH(Fortran)

AC_MSG_CHECKING([for Fortran 2003 flush instruction])
AC_LINK_IFELSE([AC_LANG_PROGRAM([],
               [[      flush(6) ]])],
               [ cs_fc_flush=yes ],
               [ cs_fc_flush=no])
AC_MSG_RESULT($cs_fc_flush)

AC_LANG_POP([Fortran])

if test "x$cs_fc_flush" = "xyes"; then
  if test "x$cs_ibm_bg_type" != "x" ; then
    FCFLAGS="${FCFLAGS} -WF,-D_CS_FC_HAVE_FLUSH"
  else
    FCFLAGS="${FCFLAGS} -D_CS_FC_HAVE_FLUSH"
  fi
fi

unset $cs_fc_flush

])dnl
