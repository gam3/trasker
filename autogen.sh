#! /bin/sh

#find * -name autogen.sh -exec {} \;

set -x
aclocal
#libtoolize --force --copy
autoheader
automake --add-missing --copy -Wno-portability
autoconf

