#!/bin/sh

# Generously borrowed from Daniel Elstner's regexxer

# Be Bourne compatible. (stolen from autoconf)
if test -n "${ZSH_VERSION+set}" && (emulate sh) >/dev/null 2>&1; then
  emulate sh
  NULLCMD=:
  # Zsh 3.x and 4.x performs word splitting on ${1+"$@"}, which
  # is contrary to our usage.  Disable this feature.
  alias -g '${1+"$@"}'='"$@"'
elif test -n "${BASH_VERSION+set}" && (set -o posix) >/dev/null 2>&1; then
  set -o posix
fi

PROJECT=jabberoo
MIN_AUTOMAKE_VERSION=1.4

srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

origdir=`pwd`
cd "$srcdir"

ACLOCAL_FLAGS="$ACLOCAL_FLAGS"
AUTOMAKE_FLAGS="--add-missing --gnu $AUTOMAKE_FLAGS"

if test -z "$AUTOGEN_SUBDIR_MODE" && test "x$*" = x
then
  echo "I am going to run ./configure with no arguments - if you wish "
  echo "to pass any to it, please specify them on the $0 command line."
fi

autoconf=autoconf
autoheader=autoheader
aclocal=
automake=
auto_version=0

# sed magic to transform a version string into a mathematical expression.
# For instance, "1.7.2" becomes "1 \* 1000000 + 7 \* 1000 + 02".  This string
# can be fed to 'eval expr' in order to compare version numbers.
#
d='[0123456789]'
get_version='s/^.*(GNU automake) \('$d$d'*\)\.\('$d$d'*\)\.*\('$d'*\).*$'
get_version=$get_version'/\1 \\* 1000000 + \2 \\* 1000 + 0\3/p'

for suffix in -1.4 -1.5 -1.6 -1.7 -1.8 -1.9 ""
do
  aclocal_version=`aclocal$suffix --version </dev/null 2>&1 | sed -n "$get_version"`
  automake_version=`automake$suffix --version </dev/null 2>&1 | sed -n "$get_version"`

  if test -n "$aclocal_version" && \
     test "x$aclocal_version" = "x$automake_version" && \
     eval "expr $automake_version \\>= $auto_version" >/dev/null
  then
    auto_version=$automake_version
    aclocal=aclocal$suffix
    automake=automake$suffix
  fi
done

min_auto_version=`echo "(GNU automake) $MIN_AUTOMAKE_VERSION" | sed -n "$get_version"`

if eval "expr $auto_version \\< $min_auto_version" >/dev/null
then
  echo "Sorry, at least automake $MIN_AUTOMAKE_VERSION is required to configure $PROJECT."
  exit 1
fi

#rm -f intltool-extract.in intltool-merge.in intltool-update.in po/Makefile.in.in
rm -f config.guess config.sub depcomp install-sh missing
rm -f config.cache acconfig.h
rm -rf autom4te.cache

#WARNINGS=all
#export WARNINGS

set_option=:
test -z "${BASH_VERSION+set}" || set_option=set

$set_option -x

#glib-gettextize --copy          || exit 1
#intltoolize --automake          || exit 1
"$aclocal" $ACLOCAL_FLAGS       || exit 1
"$autoheader"                   || exit 1
"$automake" $AUTOMAKE_FLAGS     || exit 1
"$autoconf"                     || exit 1
cd "$origdir"                   || exit 1

if test -z "$AUTOGEN_SUBDIR_MODE"
then
  "$srcdir/configure" ${1+"$@"} || exit 1
  $set_option +x
  echo
  echo "Now type 'make' to compile $PROJECT."
fi

exit 0
