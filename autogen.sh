#! /bin/sh

#find * -name autogen.sh -exec {} \;

for name in $(find . -name Makefile.am); do file=$(dirname $name)/$(basename $name .am); rm -f $file $file.in; done

#pkg-config is required

set -x
aclocal
#libtoolize --force --copy
autoheader
automake --add-missing --copy -Wno-portability
autoreconf --force --install

